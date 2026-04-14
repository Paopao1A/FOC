#include "task.h"

SPWM spwm = SPWM_DEFAULTS;
SVPWM svpwm = SVPWM_DEFAULTS;
IPARK ipark = IPARK_DEFAULTS;
ICLARKE iclarke = ICLARKE_DEFAULTS;
CLARKE clarke = CLARKE_DEFAULTS;
PARK park = PARK_DEFAULTS;

/* Clarke 变换：三相静止坐标 -> 两相静止坐标(alpha,beta) */
void ClarkeCale(M_CLARKE pv)
{
    if (pv == NULL)
    {
        return;
    }

    pv->Alpha = CLARKE_COEF_1 * pv->Uu + CLARKE_COEF_2 * pv->Uv + CLARKE_COEF_2 * pv->Uw;
    pv->Beta = CLARKE_COEF_3 * (pv->Uv - pv->Uw);
}

/* 逆 Clarke 变换：两相静止坐标(alpha,beta) -> 三相静止坐标 */
void IClarkeCale(M_ICLARKE pv)
{
    if (pv == NULL)
    {
        return;
    }

    pv->Uu = INV_CLARKE_COEF_1 * pv->Alpha;
    pv->Uv = INV_CLARKE_COEF_2 * pv->Alpha + INV_CLARKE_COEF_3 * pv->Beta;
    pv->Uw = INV_CLARKE_COEF_2 * pv->Alpha + INV_CLARKE_COEF_4 * pv->Beta;
}

/* Park 变换：alpha,beta -> d,q（旋转坐标） */
void ParkCale(M_PARK pv)
{
    if (pv == NULL)
    {
        return;
    }

    float cos_theta = arm_cos_f32(pv->Theta);
    float sin_theta = arm_sin_f32(pv->Theta);

    pv->Ds = cos_theta * pv->Alpha + sin_theta * pv->Beta;
    pv->Qs = -sin_theta * pv->Alpha + cos_theta * pv->Beta;
}

/* 逆 Park 变换：d,q -> alpha,beta */
void IPARK_Cale(M_IPARK pv)
{
    if (pv == NULL)
    {
        return;
    }

    float cos_theta = arm_cos_f32(pv->Theta);
    float sin_theta = arm_sin_f32(pv->Theta);

    pv->Alpha = cos_theta * pv->Ds - sin_theta * pv->Qs;
    pv->Beta = sin_theta * pv->Ds + cos_theta * pv->Qs;
}

/* 经典 SPWM：dq -> alpha,beta -> 三相占空比 */
void SPWM_Calc(M_SPWM pv)
{
    if (pv == NULL)
    {
        return;
    }

    ipark.Theta = pv->Theta;
    ipark.Ds = pv->Ud;
    ipark.Qs = pv->Uq;
    IPARK_Cale(&ipark);

    iclarke.Alpha = ipark.Alpha;
    iclarke.Beta = ipark.Beta;
    IClarkeCale(&iclarke);

    iclarke.Uu += Ubus / 2.0f;
    iclarke.Uv += Ubus / 2.0f;
    iclarke.Uw += Ubus / 2.0f;

    if (iclarke.Uu < 0.0f) iclarke.Uu = 0.0f;
    if (iclarke.Uu > pv->Umax) iclarke.Uu = pv->Umax;

    if (iclarke.Uv < 0.0f) iclarke.Uv = 0.0f;
    if (iclarke.Uv > pv->Umax) iclarke.Uv = pv->Umax;

    if (iclarke.Uw < 0.0f) iclarke.Uw = 0.0f;
    if (iclarke.Uw > pv->Umax) iclarke.Uw = pv->Umax;

    iclarke.PWMu = (uint16_t)(TIM1_ARR_VALUE - iclarke.Uu / Ubus * TIM1_ARR_VALUE);
    iclarke.PWMv = (uint16_t)(TIM1_ARR_VALUE - iclarke.Uv / Ubus * TIM1_ARR_VALUE);
    iclarke.PWMw = (uint16_t)(TIM1_ARR_VALUE - iclarke.Uw / Ubus * TIM1_ARR_VALUE);

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, iclarke.PWMu);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, iclarke.PWMv);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, iclarke.PWMw);
}

void SVPWM_Calc(M_SVPWM pv)
{
    if (pv == NULL)
    {
        return;
    }

    /* 1) dq -> alpha,beta */
    ipark.Theta = pv->Theta;
    ipark.Ds = pv->Ud;
    ipark.Qs = pv->Uq;
    IPARK_Cale(&ipark);

    /* 2) 按参考工程的 u1/u2/u3 法做扇区判断与 Tabc 计算 */
    float Ualpha = ipark.Alpha;
    float Ubeta = ipark.Beta;
    float u1 = Ubeta;
    float u2 = Ubeta * 0.5f + Ualpha * 0.8660254f;
    float u3 = u2 - u1;

    uint8_t sector = 3;
    float Ta = 0.0f, Tb = 0.0f, Tc = 0.0f;

    sector = (u2 > 0.0f) ? (uint8_t)(sector - 1) : sector;
    sector = (u3 > 0.0f) ? (uint8_t)(sector - 1) : sector;
    sector = (u1 < 0.0f) ? (uint8_t)(7 - sector) : sector;

    if ((sector == 1) || (sector == 4))
    {
        Ta = u2;
        Tb = u1 - u3;
        Tc = -u2;
    }
    else if ((sector == 2) || (sector == 5))
    {
        Ta = u3 + u2;
        Tb = u1;
        Tc = -u1;
    }
    else if ((sector == 3) || (sector == 6))
    {
        Ta = u3;
        Tb = -u3;
        Tc = -(u1 + u2);
    }

    /* 3) 将电压量归一化到 duty：Ta/Tb/Tc -> [0,1] */
    /* 1/(Ubus/sqrt(3)) = sqrt(3)/Ubus */
    float km_backw = (Ubus > 0.0f) ? (1.7320508f / Ubus) : 0.0f;
    float duty_u = Ta * km_backw * 0.5f + 0.5f;
    float duty_v = Tb * km_backw * 0.5f + 0.5f;
    float duty_w = Tc * km_backw * 0.5f + 0.5f;

    if (duty_u < 0.0f) duty_u = 0.0f; else if (duty_u > 1.0f) duty_u = 1.0f;
    if (duty_v < 0.0f) duty_v = 0.0f; else if (duty_v > 1.0f) duty_v = 1.0f;
    if (duty_w < 0.0f) duty_w = 0.0f; else if (duty_w > 1.0f) duty_w = 1.0f;

    /* 4) 输出到定时器：本工程是 PWM2 模式，CCR 与 duty 反向 */
    uint16_t PWMu = (uint16_t)((1.0f - duty_u) * TIM1_ARR_VALUE);
    uint16_t PWMv = (uint16_t)((1.0f - duty_v) * TIM1_ARR_VALUE);
    uint16_t PWMw = (uint16_t)((1.0f - duty_w) * TIM1_ARR_VALUE);

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, PWMu);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, PWMv);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, PWMw);
}
