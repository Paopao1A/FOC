#include "task.h"

void VrTask(void)
{
    float Vr_TargetSpeed;
    switch(Motor_CurrentState.state)
    {
        case TO_ZERO:
            break;

        case SPEED_OPENLOOP:
            Vr_TargetSpeed=AdcParaFinal.Vr / 4095.0f * Vr_Nmax;//把电位器数值映射到0-200rpm
            spwm.Speed=Vr_TargetSpeed;
            break;
        
        default:
            break;
    }
}

void RecevToVofa(void)
{


}

void SendToVofa(void)
{
    printf("%f,%f,%f\r\n",iclarke.Uu,iclarke.Uv,iclarke.Uw);
}

void HmiTask(void)
{
    uint8_t KeyNum;
    KeyNum=Key_NumGet();

    switch(KeyNum)
    {
        case KEY2_SHORT_PRESS:
		    Motor_Start();					 // 启动PWM
		    Motor_CurrentState.state = SPEED_OPENLOOP; // 无感，速度开环状态
            break;
    }

}
