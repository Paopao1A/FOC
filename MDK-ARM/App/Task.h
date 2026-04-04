#ifndef _TASK_H_
#define _TASK_H_

#include <stdlib.h>
#include <stdio.h>
#include "delay.h"
#include "main.h"
#include "usart.h"
#include "tim.h"
#include "adc.h"
#include "stm32f4xx_it.h"
#include "Key.h"
#include "Timer.h"
#include "AD.h"
#include "Motor_Config.h"
#include "App.h"
#include "Uart.h"
#include "Motor_Ctrl.h"
#include "FOC_Sensor.h"
#include "FOC.h"
#include "LED.h"
#include "arm_math.h"
#include "FOC_Sensorless.h"
#include "PID.h"

typedef struct
{
    uint8_t state;//任务状态,0是挂起，1是就绪
    uint16_t count;//时间片计数
    uint16_t reload;//时间片重载值
    void (*TaskHandle)(void);//任务处理函数
}task_t;

void task_init(void);
void task_loop(void);

#endif
