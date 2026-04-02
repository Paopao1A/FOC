#ifndef _MOTOR_CONFIG_H_
#define _MOTOR_CONFIG_H_

// 电机参数
#define MOTOR_P   10         // 电机极对数
#define MOTOR_Ls  0.0000423f // 电机自感
#define MOTOR_Rs  0.1f       // 电机内阻
#define Ubus      12.0f      // 电源电压
#define Nmax      7800       // 最大转速7800RPM(转/分钟)

// 模式选择
#define VR_OR_PC    1        // 1: VR电位器进行修改; 0: PC上位机进行修改
#define spwm_svpwm  1        // 1: 采用SPWM; 0: 采用SVPWM

//定时器参数
#define TIM1_ARR_VALUE 8400  // 定时器1的ARR值，用于生成对称PWM
#define TIM1_FRQ       20000        // 定时器1的中断频率20kHz,也就是0.05ms执行一次

#define Vr_Nmax        200      //电位器映射到转速的最大值
#define PI			3.14159f	//Π值

void Motor_Config_Init(void);

#endif
