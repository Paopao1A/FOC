#ifndef _MOTOR_CONFIG_H_
#define _MOTOR_CONFIG_H_

// 电机参数
#define MOTOR_P   10         // 电机极对数
#define MOTOR_Ls  0.0000423f // 电机自感
#define MOTOR_Rs  0.1f       // 电机内阻
#define Ubus      12.0f      // 电源电压
#define Nmax      7800       // 最大转速7800RPM(转/分钟)
#define PI		  3.14159f	//Π值

// 模式选择
#define VR_OR_PC    1        // 1: VR电位器进行修改; 0: PC上位机进行修改
#define spwm_svpwm  1        // 1: 采用SPWM; 0: 采用SVPWM

//定时器参数
#define TIM1_ARR_VALUE 8400  // 定时器1的ARR值，用于生成对称PWM
#define TIM1_FRQ       20000        // 定时器1的中断频率20kHz,也就是0.05ms执行一次
#define TS 						 0.00005f      // SVPWM/SPWM计算的周期
#define SpeedCount     4     // 定时器1中断计数器，4次中断计算一次转速，也就是0.2ms计算一次转速
#define ENCODER_FRQ   (TIM1_FRQ / SpeedCount) * 60 // TIM1_FRQ/SpeedCount*60

// 编码器参数
#define ENCODE_LINES /******************/ 1024
#define ENCODE_PULSE /******************/ (ENCODE_LINES * 4) // ENCO_LINES*4
#define TIM3_PULSE /********************/ ENCODE_PULSE       // 定时器3重装载值

//滤波参数
#define LPF_U_A /***********************/ 0.015465039f // 截止频率100Hz
#define LPF_U_B /***********************/ 0.984534961f

#define LPF_I_RUN_A /*******************/ 0.37699f // 截止频率1200Hz
#define LPF_I_RUN_B /*******************/ 0.62301f

#define LPF_I_STOP_A /******************/ 0.00314159265f // 截止频率10Hz
#define LPF_I_STOP_B /******************/ 0.99685840735f

// 速度开环参数
#define OPENSPD_REF_MAX        200      //电位器映射到转速的最大值

// 速度环参数
#define SPD_REF_TIMER /*****************/ 30   // 定义目标速度改变的梯度时间
#define SPD_REF_OD /********************/ 5.0f // 定义目标速度改变的梯度值
#define SPD_REF_MAX /*******************/ 2000 // 目标速度上限值，机械角速度

// 速度电流环参数
#define CUR_REF_TIMER /*****************/ 30   // 定义目标电流改变的梯度时间
#define CUR_REF_OD /********************/ 5.0f // 定义目标电流改变的梯度值
#define CUR_REF_MAX /*******************/ 2000 // 目标电流上限值，机械角速度

// ADC采样参数
#define ADC_REFV /**********************/ 3.3f        // unit:V, ADC参考电压
#define ADC_REF_VALUE /*****************/ 4095        // 12-bit ADC
#define I_GAIN /************************/ 0.018315f   // 电流计算系数  (3.3/4095) / 0.044 其中0.044为电流传感器系数
#define VBUS_GAIN /******************* */ 0.0088653554f // 母线电压计算系数 (3.3/4095) / (4.7/(4.7+47)) 其中4.7K和47K为分压电阻阻值

void Motor_Config_Init(void);

#endif
