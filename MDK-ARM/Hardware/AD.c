#include "task.h"

uint16_t ADC_Value[5]; // 定义一个数组用于存储ADC转换结果
static AdcPara_t AdcPara=ADC_Para_DEFAULTS; // 定义一个结构体变量用于存储ADC相关参数
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
	AdcParaFinal.Vr = ADC_Value[4];
}

void ADC_Filter(void)
{

}

//获取ADC数据并且进行滤波处理
void ADC_Get_Data(void)
{
    ADC_Get_Origin();
    ADC_Filter();

}
