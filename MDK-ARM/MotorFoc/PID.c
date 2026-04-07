#include "task.h"

PID_t Speed_PID;
void PID_Culculate(PID_t* PID)
{
	PID->error_now=PID->target-PID->actual;
	PID->error_accumlation+=PID->error_now;
	
	if(PID->error_accumlation>PID->accumlation_max) //积分限幅
		PID->error_accumlation=PID->accumlation_max;
	else if(PID->error_accumlation<PID->accumlation_min)
		PID->error_accumlation=PID->accumlation_min;

	PID->out=PID->kp*PID->error_now+PID->ki*PID->error_accumlation+PID->kd*(PID->error_now-PID->error_last);

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
    Speed_PID.accumlation_max=10000.0f;//积分限幅先调大，后续PID调好之后看波形再调整合适的幅值，PID参数在上位机接受任务调整
    Speed_PID.accumlation_min=-10000.0f;
	  Speed_PID.kp=0.01f;

}
