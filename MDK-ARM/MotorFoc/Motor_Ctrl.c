#include "task.h"


Motor_CurrentState_t Motor_CurrentState=Motor_CurrentState_DEFAULTS; // 初始化电机状态结构体变量


void AxisDq(void)
{
	clarke.Uu=AdcParaFinal.CurU;
	clarke.Uv=AdcParaFinal.CurV;
	clarke.Uw=AdcParaFinal.CurW;
	ClarkeCale(&clarke);//进行Clarke变换，得到Alpha和Beta分量

	park.Alpha = clarke.Alpha;
	park.Beta = clarke.Beta;
	park.Theta = Motor_CurrentState.E_theta;
	ParkCale(&park);//进行Park变换，得到d轴和q轴分量

	Motor_CurrentState.Iq=park.Qs;//q轴电流分量
	Motor_CurrentState.Id=park.Ds;//d轴电流分量
}

//使用的是PWM2模式，也就是高于CCR输出高电平用于生成对称PWM
void Motor_Stop(void)
{
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    // 关闭H1N/CH2N/CH3N
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);

	//设置占空比为0，确保所有PWM输出为低电平
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, TIM1_ARR_VALUE);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, TIM1_ARR_VALUE);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, TIM1_ARR_VALUE);
}

// 电机启动函数，开启PWM输出
void Motor_Start(void)
{
	// Fault_Detection();				// 故障检测
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
				Speed_Openloop(spwm.Speed);
				SPWM_Calc(&spwm);
				break;
			
			case SPEED_LOOP:
				if(RotationDirection==Anticlockwise)
				{
					#if spwm_svpwm
					Speed_Closeloop(spwm.Speed);//逆时针旋转
					SPWM_Calc(&spwm);
					#else
					Speed_Closeloop(svpwm.Speed);//逆时针旋转
					SVPWM_Calc(&svpwm);
					#endif
				}
				else
				{
					#if spwm_svpwm
					Speed_Closeloop(-spwm.Speed);//顺时针旋转
					SPWM_Calc(&spwm);
					#else
					Speed_Closeloop(-svpwm.Speed);//顺时针旋转
					SVPWM_Calc(&svpwm);
					#endif
				}
				break;
			
			case CUR_LOOP:
				if(RotationDirection==Anticlockwise)
				{
					#if spwm_svpwm
					SpdCur_Closeloop(spwm.Speed);//逆时针旋转
					SPWM_Calc(&spwm);
					#else
					SpdCur_Closeloop(svpwm.Speed);//逆时针旋转
					SVPWM_Calc(&svpwm);
					#endif
				}
				else
				{
					#if spwm_svpwm
					SpdCur_Closeloop(-spwm.Speed);//顺时针旋转
					SPWM_Calc(&spwm);
					#else
					SpdCur_Closeloop(-svpwm.Speed);//顺时针旋转
					SVPWM_Calc(&svpwm);
					#endif
				}
			//Current_Closeloop_Test();
				break;

			case POS_LOOP:
				#if spwm_svpwm
				PosCur_Closeloop(spwm.M_Theta);
				SPWM_Calc(&spwm);
				#else
				PosCur_Closeloop(svpwm.M_Theta);
				SVPWM_Calc(&svpwm);
				#endif
				break;
			
			case STOP:
				Motor_Stop();
				break;			
			
			default:
				break;


		}
}

