#include "task.h"

void MotorAngleGet(void)
{

}


void FocTimCallback(void)// FOC定时器回调函数,TIM1的更新中断会调用这个函数,频率20kHz,也就是0.05ms执行一次
{
	ADC_Get_Data();			//获取ADC数据
	Motor_StateChoose();	//根据当前状态选择电机控制模式
}