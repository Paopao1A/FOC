#include "task.h"

void PostionToZeroDouble(void)
{
    /* 启动PWM输出 */
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);

#if spwm_svpwm
    spwm.Ud=0;
    spwm.Uq=0.5f;
    SPWM_Calc(&spwm);
    HAL_Delay(1000);

    spwm.Ud=0.5f;
    spwm.Uq=0;
    SPWM_Calc(&spwm);
    HAL_Delay(1000);//二次调零，最后使Uα与Ud重合
#else
    svpwm.Ualpha=0;
    svpwm.Ubeta=0.5f;
    SVPWM_Calc(&svpwm);
    HAL_Delay(1000);

    svpwm.Ualpha=0.5f;
    svpwm.Ubeta=0;
    SVPWM_Calc(&svpwm);
    HAL_Delay(1000);//二次调零，最后使Uα与Ud重合
#endif

    __HAL_TIM_SetCounter(&htim3, 0);//清空编码器的值
    Motor_CurrentState.speed_M_rpm=0;//此时机械角度设置为0
    Motor_CurrentState.speed_E_rpm=0;//此时电角度设置为0
    Motor_Stop();
}
