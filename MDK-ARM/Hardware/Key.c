#include "task.h"

Key_List_t Key_List[]=
{
    {GPIOB, GPIO_PIN_12},   // key1
	{GPIOB, GPIO_PIN_13},   // key2
	{GPIOB, GPIO_PIN_14},   // key3
    {GPIOB, GPIO_PIN_11},   // ONOFF
};


void Key_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitTypeDef gpioInitStruct;
	gpioInitStruct.Mode = GPIO_MODE_INPUT;
	gpioInitStruct.Pull = GPIO_PULLUP;
    gpioInitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 |GPIO_PIN_14 | GPIO_PIN_11;//KEY1,KEY2,KEY3,开关键
    HAL_GPIO_Init(GPIOB, &gpioInitStruct);
}


/*************************按键状态机************************/

Key_Info_t Key_Info[KEY_NUM_MAX];

static uint8_t Key_Scan(uint8_t KeyIndex)
{   
    uint64_t curSysTime;
    uint8_t KeyPress;
    KeyPress = HAL_GPIO_ReadPin(Key_List[KeyIndex].gpio, Key_List[KeyIndex].pin);

    switch (Key_Info[KeyIndex].keyState)
    {
        case Key_Release:
            if(!KeyPress)//按键按下
            {
                Key_Info[KeyIndex].keyState=Key_Confirm;//按键按下，状态切换为消抖确认
                Key_Info[KeyIndex].FirstIoChangeSysTime=HAL_GetTick();//记录第一次IO状态改变的时间戳
                break;
            }

            if (Key_Info[KeyIndex].SingleClickNum != 0)
						{
							curSysTime = HAL_GetTick();
							if (curSysTime - Key_Info[KeyIndex].FirstReleaseSysTime > DOUBLE_CLICK_INTERVAL_TIME)
							{
								Key_Info[KeyIndex].SingleClickNum = 0;
								return (KeyIndex + 1); //  返回单击按键码值，四个按键短按对应0x01, 0x02, 0x03, 0x04
							}
						}
						break;

        case Key_Confirm:
            if(!KeyPress)
            {
                curSysTime = HAL_GetTick();//获取当前系统时间戳

                if(curSysTime-Key_Info[KeyIndex].FirstIoChangeSysTime>CONFIRM_TIME)
                {
                    Key_Info[KeyIndex].keyState=Key_ShortPress;//按键按下，状态切换为短按
                }
						}
            else
            {
              Key_Info[KeyIndex].keyState=Key_Release;//恢复初始状态
            }
            break;
        
        case Key_ShortPress:
            if(KeyPress)//按键松开
            {
                Key_Info[KeyIndex].keyState = Key_Release;
                Key_Info[KeyIndex].SingleClickNum++;//单击次数计数器增加
                if (Key_Info[KeyIndex].SingleClickNum == 1)
								{
									Key_Info[KeyIndex].FirstReleaseSysTime = HAL_GetTick();
									break;
								}
                else
								{
									curSysTime = HAL_GetTick();
									if (curSysTime - Key_Info[KeyIndex].FirstReleaseSysTime <= DOUBLE_CLICK_INTERVAL_TIME) //  处于双击时间间隔窗，返回双击码值
									{
										Key_Info[KeyIndex].SingleClickNum = 0;
										return (KeyIndex + 0x51); //  返回按键码值，四个按键双击对应0x51, 0x52, 0x53, 0x54
									}
								}
            }
            else
            {
                curSysTime = HAL_GetTick();//获取当前系统时间戳
                if(curSysTime-Key_Info[KeyIndex].FirstIoChangeSysTime>LONGPRESS_TIME)
                {
                    Key_Info[KeyIndex].keyState=Key_LongPress;//按键按下，状态切换为长按
                }
            }
            break;
            
        case Key_LongPress:
            if (KeyPress)//按键松开
						{
							Key_Info[KeyIndex].keyState = Key_Release;
							return (KeyIndex + 0x81); //返回按键码值，四个按键长按对应0x81, 0x82, 0x83, 0x84
						}
						break;

        default:
            Key_Info[KeyIndex].keyState = Key_Release;
            break;
    }

    return 0;

}

uint8_t Key_NumGet(void)
{
    uint8_t res = 0;
    for(uint8_t i=0;i<KEY_NUM_MAX;i++)
    {
        res=Key_Scan(i);//轮询四个按键状态，如果检测到某个按键直接返回索引
        if(res!=0)
        {
            break;
        }
    }
    return res;
}
