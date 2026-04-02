#include "task.h"

void Motor_Config_Init(void)
{
    Motor_CurrentState.P = MOTOR_P;	 // 电机极对数
	Motor_CurrentState.Rs = MOTOR_Rs; // 电机电阻
	Motor_CurrentState.Ls = MOTOR_Ls; // 电机相电感

	spwm.Theta = 0;
}
