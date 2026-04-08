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

void Speed_Closeloop(float TargetSpeed)//速度闭环，电流开环
{
    static uint8_t Speed_CulCount=0;
    static float current_ref = 0;
    Speed_CulCount++;

    if(Speed_CulCount == 20)//FOC定时器20khz，我们设定0.2ms获取一次速度，1ms计算一次PID
    {
        // 速度梯度计算，限制每次速度变化
        float max_rate = 5.0f;  // 最大速度变化率，单位：rpm/ms
        float error = TargetSpeed - current_ref;
        
        if (error > max_rate)
            current_ref += max_rate;
        else if (error < -max_rate)
            current_ref -= max_rate;
        else
            current_ref = TargetSpeed;
        
	  Speed_PID.target=current_ref;//使用平滑后的目标速度
        Speed_PID.actual=Motor_CurrentState.speed_M_rpm;

        PID_Culculate(&Speed_PID);

        spwm.Theta=Motor_CurrentState.E_theta;//计算SPWM的时候要用电角度计算
        spwm.Uq=Speed_PID.out;
        spwm.Ud=0.0f;

        Speed_CulCount=0;
    }
    
}

void SpdCur_Closeloop(float TargetSpeed)//速度电流串级闭环
{
    static uint8_t SpdCur_CulCount=0;
    SpdCur_CulCount++;
	static float SpdCur_ref = 0;

    if(SpdCur_CulCount == 20)//外环速度环1ms周期
    { 
        // 速度梯度计算，限制每次速度变化
        float max_rate = 5.0f;  // 最大速度变化率，单位：rpm/ms
        float error = TargetSpeed - SpdCur_ref;
        
        if (error > max_rate)
            SpdCur_ref += max_rate;
        else if (error < -max_rate)
            SpdCur_ref -= max_rate;
        else
            SpdCur_ref = TargetSpeed;
        
	    SpdCur_PID.target=SpdCur_ref;//使用平滑后的目标速度
        SpdCur_PID.actual=Motor_CurrentState.speed_M_rpm;

        PID_Culculate(&SpdCur_PID);

        SpdCur_CulCount=0;
    }

    // 内环电流环周期0.05ms，也就是FOC定时器周期，保证电流环的快速响应
    Cur_Id_PID.target=0.0f;//d轴电流保持在0，q轴电流由速度环输出控制
    Cur_Id_PID.actual=Motor_CurrentState.Id;//电流环的实际值是Id
    PID_Culculate(&Cur_Id_PID);

    Cur_Iq_PID.target=SpdCur_PID.out;//将速度环的输出作为电流环的目标值，也就是q轴电流的目标值
    Cur_Iq_PID.actual=Motor_CurrentState.Iq;//电流环的实际值是Iq
    PID_Culculate(&Cur_Iq_PID);

    spwm.Uq=Cur_Iq_PID.out;
    spwm.Ud=Cur_Id_PID.out;
    spwm.Theta=Motor_CurrentState.E_theta;//计算SPWM的时候要用电角度
}

void Current_Closeloop_Test(void)
{
    // 完全抛弃外层速度环，直接给固定的 Iq 和 Id 目标值
    // 给 0.5A 让他产生一个恒定的推力
    Cur_Iq_PID.target = 0.5f; 
    Cur_Id_PID.target = 0.0f; 

    Cur_Iq_PID.actual = Motor_CurrentState.Iq;
    Cur_Id_PID.actual = Motor_CurrentState.Id;

    PID_Culculate(&Cur_Iq_PID);
    PID_Culculate(&Cur_Id_PID);

    spwm.Uq = Cur_Iq_PID.out;
    spwm.Ud = Cur_Id_PID.out;
    spwm.Theta = Motor_CurrentState.E_theta;
}
