#include "task.h"


Motor_CurrentState_t Motor_CurrentState=Motor_CurrentState_DEFAULTS; // 定义一个结构体变量用于存储电机当前状态


void AxisDq(void)
{
	clarke.Uu=AdcParaFinal.CurU;
	clarke.Uv=AdcParaFinal.CurV;
	clarke.Uw=AdcParaFinal.CurW;
	ClarkeCale(&clarke);//进行克拉克变换，得到αβ坐标系的电流值

	park.Alpha = clarke.Alpha;
	park.Beta = clarke.Beta;
	park.Theta = Motor_CurrentState.E_theta;
	ParkCale(&park);//进行PARK变换，得到DQ坐标系的电流值

	Motor_CurrentState.Iq=park.Qs;//更新电流值
	Motor_CurrentState.Id=park.Ds;//更新电流值
}

//使用的是PWM2模式，也就是高于CCR输出高电平用于生成对称PWM
void Motor_Stop(void)
{
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    // 禁用互补通道（CH1N/CH2N/CH3N，开启互补通道必须加）
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);

    // 步骤2：（双重保险）将比较值设为ARR（PWM模式2下输出低电平）
    // 注意：此时通道已禁用，这一步是防止后续误开启通道时输出高电平
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, TIM1_ARR_VALUE);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, TIM1_ARR_VALUE);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, TIM1_ARR_VALUE);
}

// 启动电机
void Motor_Start(void)
{
	// Fault_Detection();				//启动前先进行故障检测
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
}

void Motor_StateChoose(void)
{
    switch(Motor_CurrentState.state)
		{
			case TO_ZERO:
				break;
			
			case SPEED_OPENLOOP:
				Speed_Openloop(spwm.Speed);//机械角度转速r/mins
				SPWM_Calc(&spwm);
				break;
			
			case SPEED_LOOP:
				Speed_Closeloop(spwm.Speed);//机械角度转速r/min
			  	SPWM_Calc(&spwm);
				break;
			
			case CUR_LOOP:
				SpdCur_Closeloop(spwm.Speed);//机械角度转速r/min
			//Current_Closeloop_Test();
			  	SPWM_Calc(&spwm);
				break;
			
			case STOP:
				Motor_Stop();
				break;			
			
			default:
				break;


		}
}
