#ifndef _TIMER_H_
#define _TIMER_H_

typedef struct
{
	float Last_EncoderCnt;             // 上一次的计数器值
	float Current_EncoderCnt;             // 当前计数器值

    float Current_M_Speed;             // 当前机械角度转速
    float Filter_M_Speed;           // 滤波之后的机械角度转速
}Encoder_Data_t;

void FocTimCallback(void);

#endif
