#include "task.h"

static float Speed_to_Theta(float speed)//传入速度为转/分钟，返回角度为弧度
{
    return speed * PI / 30 * MOTOR_P ;//转/分钟转换为弧度/秒
}

void Speed_Openloop(float TargetSpeed)
{
	  static uint32_t timecount;
		timecount++;

    if(timecount==20)//1ms更新一次角度
    {
        spwm.Theta += Speed_to_Theta(TargetSpeed) * (float)timecount / TIM1_FRQ;//更新角度，按周期计算每周期的角度变化
        if(spwm.Theta > 2 * PI)
        {
            spwm.Theta -= 2 * PI;
        }   
        else if(spwm.Theta < 0)
        {
            spwm.Theta += 2 * PI;
        }
				timecount=0;
				
				spwm.Ud=0.0f;
				spwm.Uq=1.0f;
    }
}
