#include "task.h"


SPWM spwm=SPWM_DEFAULTS; // 初始化SPWM结构体变量
SVPWM svpmw=SVPWM_DEFAULTS; // 初始化SVPWM结构体变量
IPARK ipark=IPARK_DEFAULTS; // 初始化IPARK结构体变量
ICLARKE iclarke=ICLARKE_DEFAULTS; // 初始化ICLARKE结构体变量
CLARKE clarke=CLARKE_DEFAULTS; // 初始化CLARKE结构体变量
PARK park=PARK_DEFAULTS; // 初始化PARK结构体变量

//clarke变换
void ClarkeCale(M_CLARKE pv)
{
    // 空指针检查
    if (pv == NULL)
    {
        return;
    }

    // 标准Clarke变换公式（利用三相电流平衡：Iw = -Iu - Iv）
    // α = Iu
    // β = (2Iv + Iu) / √3 = Iu*(1/√3) + Iv*(2/√3)
    pv->Alpha = CLARKE_COEF_1 * pv->Uu + CLARKE_COEF_2 * pv->Uv + CLARKE_COEF_2 * pv->Uw;
    pv->Beta  = CLARKE_COEF_3 * (pv->Uv - pv->Uw);
}

//Clarke逆变换
void IClarkeCale(M_ICLARKE pv)
{
    // 空指针检查（避免运行时崩溃）
    if (pv == NULL)
    {
        return;
    }

    // 执行逆克拉克变换计算
    pv->Uu = INV_CLARKE_COEF_1 * pv->Alpha;                    // Uu = α
    pv->Uv = INV_CLARKE_COEF_2 * pv->Alpha + INV_CLARKE_COEF_3 * pv->Beta; // Uv = -0.5α + (√3/2)β
    pv->Uw = INV_CLARKE_COEF_2 * pv->Alpha + INV_CLARKE_COEF_4 * pv->Beta; // Uw = -0.5α - (√3/2)β
}

//park变换
void ParkCale(M_PARK pv)
{
    // 空指针检查
    if (pv == NULL)
    {
        return;
    }

    float cos_theta = arm_cos_f32(pv->Theta);
    float sin_theta = arm_sin_f32(pv->Theta);

    // 标准Park变换公式
    pv->Ds = cos_theta * pv->Alpha + sin_theta * pv->Beta;  // d轴 = α*cosθ + β*sinθ
    pv->Qs = -sin_theta * pv->Alpha + cos_theta * pv->Beta; // q轴 = -α*sinθ + β*cosθ
}

//park逆变换
void IPARK_Cale(M_IPARK pv)
{
    // 空指针检查
    if (pv == NULL)
    {
        return;
    }

    float cos_theta = arm_cos_f32(pv->Theta);
    float sin_theta = arm_sin_f32(pv->Theta);

    // 标准IPark变换公式（Park逆变换）
    pv->Alpha = cos_theta * pv->Ds - sin_theta * pv->Qs;  // α = d*cosθ - q*sinθ
    pv->Beta  = sin_theta * pv->Ds + cos_theta * pv->Qs;  // β = d*sinθ + q*cosθ
}

void SPWM_Calc(M_SPWM pv)
{
    // 空指针检查
    if (pv == NULL)
    {
        return;
    }

    ipark.Theta=pv->Theta;
    ipark.Ds=pv->Ud;
    ipark.Qs=pv->Uq;
    IPARK_Cale(&ipark);

    iclarke.Alpha=ipark.Alpha;
    iclarke.Beta=ipark.Beta;
    IClarkeCale(&iclarke);

    iclarke.Uu += Ubus/2.0f; //电压抬升形成全正的电压
    iclarke.Uv += Ubus/2.0f;
    iclarke.Uw += Ubus/2.0f;

    if (iclarke.Uu < 0.0f) iclarke.Uu = 0.0f;
    if (iclarke.Uu > pv->Umax)  iclarke.Uu = pv->Umax;

    if (iclarke.Uv < 0.0f) iclarke.Uv = 0.0f;
    if (iclarke.Uv > pv->Umax)  iclarke.Uv = pv->Umax;

    if (iclarke.Uw < 0.0f) iclarke.Uw = 0.0f;
    if (iclarke.Uw > pv->Umax)  iclarke.Uw = pv->Umax;

    iclarke.PWMu = (uint16_t)(TIM1_ARR_VALUE - iclarke.Uu / Ubus * TIM1_ARR_VALUE);//PWM模式二，高于设定值才输出高电平
    iclarke.PWMv = (uint16_t)(TIM1_ARR_VALUE - iclarke.Uv / Ubus * TIM1_ARR_VALUE);
    iclarke.PWMw = (uint16_t)(TIM1_ARR_VALUE - iclarke.Uw / Ubus * TIM1_ARR_VALUE);

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, iclarke.PWMu);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, iclarke.PWMv);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, iclarke.PWMw);
}

void SVPWM_Calc(M_SVPWM pv)
{
    // 空指针检查
    if (pv == NULL)
    {
        return;
    }
}
