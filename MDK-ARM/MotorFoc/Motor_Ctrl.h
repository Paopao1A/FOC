#ifndef _MOTOR_CTRL_H_
#define _MOTOR_CTRL_H_

#include "main.h"

typedef enum
{
    STOP=0,         // 电机停止状态
    M_ERR=11,        // 电机错误状态
    TO_ZERO=4,      // 校准状态

    /*有感FOC*/
    SPEED_LOOP=1,   // 速度闭环状态
    CUR_LOOP=2,     // 电流闭环状态
    POS_LOOP=3,     // 位置闭环状态

    /*无感FOC*/
    IF_CTRL=5,      // IF控制状态
    HFI_CTRL=6,     // HFI控制状态
    HFI_SMO=7,      // 低速HFI高速SMO状态
    HFI_FLUX=8,     // 低速HFI高速磁链状态
    SPEED_OPENLOOP=9, // 速度开环状态
}Motor_State_t;


typedef struct
{
    float M_theta;                  // 电机转子机械角度
    float E_theta;                  // 电机转子电角度
    float speed_M_rpm;              // 电机当前机械角速度	rpm/min
    float speed_E_rpm;              // 电机当前电角速度		rpm/min
    volatile Motor_State_t state;   // 电机当前所处控制状态
    float Rs;                       // 电机的相电阻
    float Ls;                       // 电机的相电感
    float Ib;                       // 电机相电流
    float Vb;                       // 电机相电压
    uint8_t P;                      // 电机极对数

}Motor_CurrentState_t;  

#define Motor_CurrentState_DEFAULTS {0.0f, 0.0f, 0.0f, 0.0f, STOP, 0.0f, 0.0f, 0.0f, 0.0f, 0} // 电机状态参数初始化

void Motor_Stop(void);
void Motor_Start(void);
void Motor_StateChoose(void);
extern Motor_CurrentState_t Motor_CurrentState; // 声明一个结构体变量用于存储电机当前状态

#endif
