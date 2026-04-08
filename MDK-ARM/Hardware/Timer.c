#include "task.h"

Encoder_Data_t Encoder_Data;


void Motor_AngleGet(void)
{
	//Motor_CurrentState.E_theta=(float)__HAL_TIM_GET_COUNTER(&htim3) / ENCODE_PULSE * 2 * PI * MOTOR_P;

	Motor_CurrentState.M_theta = (float)__HAL_TIM_GET_COUNTER(&htim3) / ENCODE_PULSE * 2.0f * PI;
  Motor_CurrentState.E_theta = fmodf(Motor_CurrentState.M_theta * MOTOR_P, 2.0f * PI);
	
	if(Motor_CurrentState.E_theta >= 2 * PI)
	{
		Motor_CurrentState.E_theta -= 2 * PI;
	}
	else if(Motor_CurrentState.E_theta < 0.0f)
	{
		Motor_CurrentState.E_theta += 2 * PI;
	}
}

void Motor_SpeedGet(void)
{
	int16_t diff;
	
	Encoder_Data.Current_EncoderCnt=__HAL_TIM_GET_COUNTER(&htim3);//获取当前编码器计数值
	diff=Encoder_Data.Current_EncoderCnt-Encoder_Data.Last_EncoderCnt;//计算前后差值
	Encoder_Data.Last_EncoderCnt=Encoder_Data.Current_EncoderCnt;//更新旧值

	/*由于物理因素限制，我们每个周期增加的电角度不可以超过180°，即机械角度不可以超过180°/极对数
	，也就是说每次读取的脉冲数不可以多于TIM3_PULSE / 2 / MOTOR_P（意思就是机械角度转半圈
	的脉冲数TIM3_PULSE / 2，再除以极对数就是电角度不超过180°的脉冲数），实际上这是我们的隐形条件
	因为我们输入的转速不可以超出这个限制，下面的这个式子只是用来判断计数器的溢出，只有溢出才会
	出现diff差值大于限制脉冲的情况，说明计数器回绕了。*/
	if(diff > (int16_t)(TIM3_PULSE / 2 / MOTOR_P))//下溢,比如从1到4095，4095-1=4094，diff=4096-4094=2
	{
		diff-=TIM3_PULSE;
	}
	else if(diff < (int16_t)(- TIM3_PULSE / 2 / MOTOR_P))//上溢
	{
		diff+=TIM3_PULSE;
	}

	Encoder_Data.Current_M_Speed = (float)diff / ENCODE_PULSE * ENCODER_FRQ;//得到转/分钟

	Encoder_Data.Filter_M_Speed=(LPF_U_A * Encoder_Data.Current_M_Speed) + (LPF_U_B * Encoder_Data.Filter_M_Speed);//一阶低通滤波

	Motor_CurrentState.speed_M_rpm=Encoder_Data.Filter_M_Speed;
	Motor_CurrentState.speed_E_rpm=Encoder_Data.Filter_M_Speed * MOTOR_P;//更新转速

}

void FocTimCallback(void)// FOC定时器回调函数,TIM1的更新中断会调用这个函数,频率20kHz,也就是0.05ms执行一次
{
	static uint8_t Count=0;
	ADC_Get_Data();			//获取ADC数据
	Motor_AngleGet();			//获取电机角度
	AxisDq();			//坐标变换	
	
	Count++;
	if(Count>=SpeedCount)		// 0.2ms计算一次电机转速，为了保证一个周期电角度不超过180
	{							//转速的峰值为 180/10*（1/0.0002）/360*60=15000rpm
		Count=0;
		Motor_SpeedGet();
	}
	Motor_StateChoose();	//根据当前状态选择电机控制模式
}
