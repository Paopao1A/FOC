#include "task.h"

PID_t Speed_PID;
PID_t Cur_Id_PID[2];
PID_t Cur_Iq_PID[2];
PID_t SpdCur_PID;
PID_t PosCur_PID;

void PID_Culculate(PID_t* PID)
{
	PID->error_now=PID->target-PID->actual;
	
	if(PID->ki!=0)
		PID->error_accumlation +=PID->ki * PID->error_now * TS;//在ki没有值的时候不进行累加，主要是为了调参过程如果突然施加ki会直接积分爆炸

	if(PID->error_accumlation>PID->accumlation_max) //积分限幅
		PID->error_accumlation=PID->accumlation_max;
	else if(PID->error_accumlation<PID->accumlation_min)
		PID->error_accumlation=PID->accumlation_min;

	PID->out=PID->kp*PID->error_now + PID->error_accumlation + PID->kd*(PID->error_now-PID->error_last);

	//采用微分先行，利用实际值的变化率来预测未来的误差，这样可以让系统响应更快更稳定，特别是对于有较大滞后或者惯性的系统
	//PID->out=PID->kp*PID->error_now+PID->ki*PID->error_accumlation-PID->kd*(PID->actual-PID->actual_last);
	
	if(PID->out>PID->outmax)
		PID->out=PID->outmax;
	else if(PID->out<PID->outmin)
		PID->out=PID->outmin;
	

	PID->error_last=PID->error_now;
	//PID->actual_last=PID->actual;
}

void PID_Init(void)
{
	//速度环参数初始化
    Speed_PID.outmax=6.0f;
    Speed_PID.outmin=-6.0f;
    Speed_PID.accumlation_max=6.0f;//积分限幅先调大，后续PID调好之后看波形再调整合适的幅值，PID参数在上位机接受任务调整
    Speed_PID.accumlation_min=-6.0f;
	Speed_PID.kp=0.015f;
	Speed_PID.ki=0.5f;

	//速度电流串级环参数初始化
	SpdCur_PID.outmax=8.0f;
	SpdCur_PID.outmin=-8.0f;
	SpdCur_PID.accumlation_max=8.0f;
	SpdCur_PID.accumlation_min=-8.0f;
	SpdCur_PID.kp=0.05f;
	SpdCur_PID.ki=0.1f;

	Cur_Id_PID[0].outmax=6.0f;
	Cur_Id_PID[0].outmin=-6.0f;
	Cur_Id_PID[0].accumlation_max=6.0f;
	Cur_Id_PID[0].accumlation_min=-6.0f;
	Cur_Id_PID[0].kp=0.5f;
	Cur_Id_PID[0].ki=1.0f;

	Cur_Iq_PID[0].outmax=6.0f;
	Cur_Iq_PID[0].outmin=-6.0f;
	Cur_Iq_PID[0].accumlation_max=6.0f;
	Cur_Iq_PID[0].accumlation_min=-6.0f;
	Cur_Iq_PID[0].kp=0.5f;
	Cur_Iq_PID[0].ki=1.0f;

	//位置电流串级环参数初始化
	PosCur_PID.outmax=10.0f;
	PosCur_PID.outmin=-10.0f;
	PosCur_PID.accumlation_max=10.0f;
	PosCur_PID.accumlation_min=-10.0f;
	PosCur_PID.kp=20.0f;
	PosCur_PID.ki=150.0f;

	Cur_Id_PID[1].outmax=6.0f;
	Cur_Id_PID[1].outmin=-6.0f;
	Cur_Id_PID[1].accumlation_max=6.0f;
	Cur_Id_PID[1].accumlation_min=-6.0f;
	Cur_Id_PID[1].kp=0.1f;
	Cur_Id_PID[1].ki=0.01f;

	Cur_Iq_PID[1].outmax=6.0f;
	Cur_Iq_PID[1].outmin=-6.0f;
	Cur_Iq_PID[1].accumlation_max=6.0f;
	Cur_Iq_PID[1].accumlation_min=-6.0f;
	Cur_Iq_PID[1].kp=0.1f;
	Cur_Iq_PID[1].ki=0.01f;
}
