#include "task.h"

uint16_t ADC_Value[5]; // 定义一个数组用于存储ADC转换结果
AdcPara_t AdcPara=ADC_Para_DEFAULTS; // 定义一个结构体变量用于存储ADC相关参数
AdcParaFinal_t AdcParaFinal=ADC_ParaFinal_DEFAULTS; // 处理之后的ADC参数
/*ADC_Value[0]*/



//获取零点的偏移
void ADC_Get_Drift(void)
{
    uint8_t i = 0;
	uint8_t j = 0;
	uint16_t Sum1 = 0;
	uint16_t Sum2 = 0;
    // 采集10次，每次采集100次，取平均值作为偏移值
	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 100; j++)
		{
			AdcPara.OffsetCurU = ADC_Value[0];
			AdcPara.OffsetCurW = ADC_Value[3];

			HAL_Delay(1);
		}
		Sum1 += AdcPara.OffsetCurU;
		Sum2 += AdcPara.OffsetCurW;
	}
    AdcParaFinal.OffsetCurU = Sum1 / 10;
    AdcParaFinal.OffsetCurW = Sum2 / 10;
}

//获取原始ADC数据
void ADC_Get_Origin(void)
{	
	AdcPara.CurU = (float)(-AdcParaFinal.OffsetCurU + ADC_Value[0]) * I_GAIN;
	AdcPara.CurW = (float)(-AdcParaFinal.OffsetCurW + ADC_Value[3]) * I_GAIN;
	AdcPara.CurV = -AdcPara.CurU - AdcPara.CurW;

	AdcPara.Vbus = (float)ADC_Value[2] * VBUS_GAIN;
	AdcPara.Temp = (float)ADC_Value[1];
	AdcPara.Vr = ADC_Value[4];
}

void ADC_Filter(void)
{
	AdcParaFinal.CurU = LPF_I_RUN_B * AdcParaFinal.CurU + LPF_I_RUN_A * AdcPara.CurU;
	AdcParaFinal.CurV = LPF_I_RUN_B * AdcParaFinal.CurV + LPF_I_RUN_A * AdcPara.CurV;
	AdcParaFinal.CurW = LPF_I_RUN_B * AdcParaFinal.CurW + LPF_I_RUN_A * AdcPara.CurW;
	AdcParaFinal.Vbus = AdcParaFinal.Vbus * LPF_I_STOP_B + AdcPara.Vbus * LPF_I_STOP_A;
	AdcParaFinal.Temp = AdcParaFinal.Temp * LPF_I_STOP_B + AdcPara.Temp * LPF_I_STOP_A;
	AdcParaFinal.Vr = AdcParaFinal.Vr * LPF_VR_B + AdcPara.Vr * LPF_VR_A;
}

//获取ADC数据并且进行滤波处理
void ADC_Get_Data(void)
{
    ADC_Get_Origin();
    ADC_Filter();
}
