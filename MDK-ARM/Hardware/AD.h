#ifndef _AD_H_
#define _AD_H_

#include "main.h"

typedef struct
{
    float Vbus;	   // 母线电压
	float Vr;	   // 电位器
	uint16_t Temp; // 温度

	float CurU; // U相电流
	float CurV; // V相电流
	float CurW; // W相电流

	int16_t OffsetCurU; // U相电流偏置值
	int16_t OffsetCurV; // V相电流偏置值
	int16_t OffsetCurW; // W相电流偏置值
}AdcPara_t;

#define ADC_Para_DEFAULTS {0, 0, 0, 0, 0, 0, 0, 0, 0} // 初始化参数

typedef struct
{
    float Vbus;	   // 母线电压
	float Vr;	   // 电位器
	uint16_t Temp; // 温度

	float CurU; // U相电流
	float CurV; // V相电流
	float CurW; // W相电流

	int16_t OffsetCurU; // U相电流偏置值
	int16_t OffsetCurV; // V相电流偏置值
	int16_t OffsetCurW; // W相电流偏置值
}AdcParaFinal_t;

#define ADC_ParaFinal_DEFAULTS {0, 0, 0, 0, 0, 0, 0, 0, 0} // 初始化参数

#define AD_Uu ADC_Value[0]		// U相电流
#define AD_Temp ADC_Value[1]	// 温度
#define AD_Ubus ADC_Value[2]	// 母线电压
#define AD_Uw ADC_Value[3]		// W相电流
#define AD_Ur ADC_Value[4]		// 电位器数值

void ADC_Get_Drift(void);
void ADC_Get_Data(void);
extern uint16_t ADC_Value[5]; // 声明一个外部变量，用于存储ADC转换结果
extern AdcParaFinal_t AdcParaFinal;

#endif
