#include "Task.h"

/************************************************************************/
/*定义任务以及相关变量*/
static task_t task[]=
{
#if VR_OR_PC
    {0,1,1,VrTask},//电位器任务
#else
    {0, 10, 10, RecevToVofa},//解析上位机数据任务
#endif
    {0, 5, 5, HmiTask},	//人机交互任务
};

static uint8_t Task_MaxNum=sizeof(task)/sizeof(task[0]);//任务数量

/************************************************************************/
/*函数调度原理就是每隔1ms调用一次TaskScheduleCb函数，
TaskScheduleCb函数会遍历所有任务，如果任务的count不为0，
则将count减1，如果count减到0了，就将任务状态置为就绪，并
重置count为reload值。然后在主循环中调用TaskHandle函数，
TaskHandle函数会遍历所有任务，如果任务状态为就绪，就执行该任务的处理函数，
并将任务状态置为挂起。这样就实现了一个简单的时间片轮转调度算法。*/
static void TaskHandle(void)//任务处理函数
{
    for(uint8_t i=0;i<Task_MaxNum;i++)
    {
        if(task[i].state==1)
        {
            task[i].state=0;//挂起任务
            if(task[i].TaskHandle==NULL)
            {
                continue;
            }
            task[i].TaskHandle();
        }    
    }

} 

static void TaskScheduleCb(void)//任务调度函数
{
    for(uint8_t i=0;i<Task_MaxNum;i++)
    {
        if(task[i].count)
        {
            task[i].count--;
            if(task[i].count==0)
            {
                task[i].state=1;//就绪任务
                task[i].count=task[i].reload;//重置计数器
            }
        }
    } 
}

/************************************************************************/
/*硬件初始化函数*/
static void Hardware_Init(void)
{
    Key_Init();
    UATR_Init();
	__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);	 // 避免中断已开启就进入中断
	HAL_TIM_Base_Start_IT(&htim1);				 // 开启定时器1中断
}

static void App_Init(void)
{
    TaskSchFunc(TaskScheduleCb); // 注册任务调度函数
    TimeSchFunc(FocTimCallback); // 注册FOC运算调度函数
}

void task_init(void)
{
    Hardware_Init();
    App_Init();
    Motor_Config_Init();

    HAL_Delay(100); // 等待系统稳定

    Motor_CurrentState.state=TO_ZERO; // 设置电机状态为校准状态
    PostionToZeroDouble(); // 调零函数
    Motor_CurrentState.state=STOP; // 设置电机状态为停止状态

 	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1 | TIM_CHANNEL_2); // 开启编码器使能函数
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_Value, 5);		  // 开启ADC-DMA转换
    ADC_Get_Drift(); // 获取相电压零点偏移值
    TurnOnLed(0); // 点亮蓝色LED,表示初始化完成
}

void task_loop(void)
{
    TaskHandle();
    SendToVofa();
}
