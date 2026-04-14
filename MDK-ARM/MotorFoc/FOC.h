#ifndef _FOC_H_
#define _FOC_H_


typedef struct
{
	float Uu;	 // 三相电压A
	float Uv;	 // 三相电压B
	float Uw;	 // 三相电压C
	float Alpha; // 二相静止坐标系 Alpha 轴
	float Beta;	 // 二相静止坐标系 Beta 轴
} CLARKE, *M_CLARKE;

typedef struct
{
	float Uu;	 // 三相电压A
	float Uv;	 // 三相电压B
	float Uw;	 // 三相电压C
	float Alpha; // 二相静止坐标系 Alpha 轴
	float Beta;	 // 二相静止坐标系 Beta 轴
	uint16_t PWMu;	// 三相电压U的PWM值
	uint16_t PWMv;	// 三相电压V的PWM值
	uint16_t PWMw;	// 三相电压W的PWM值
} ICLARKE, *M_ICLARKE;

typedef struct
{
	float Alpha; // 二相静止坐标系 Alpha 轴
	float Beta;	 // 二相静止坐标系 Beta 轴
	float Theta; // 电机电角度
	float Ds;	 // 电机二相旋转坐标系下的d轴电压
	float Qs;	 // 电机二相旋转坐标系下的q轴电压
} PARK, *M_PARK;

typedef struct
{
	float Alpha; // 二相静止坐标系 Alpha 轴
	float Beta;	 // 二相静止坐标系 Beta 轴
	float Theta; // 电机电角度
	float Ds;	 // 电机二相旋转坐标系下的d轴电压
	float Qs;	 // 电机二相旋转坐标系下的q轴电压
} IPARK, *M_IPARK;

#define CLARKE_DEFAULTS {0, 0, 0, 0, 0} // CLARK初始化参数
#define ICLARKE_DEFAULTS {0, 0, 0, 0, 0, 0, 0, 0} // ICLARK初始化参数
#define PARK_DEFAULTS {0, 0, 0, 0, 0}	// PARK初始化参数
#define IPARK_DEFAULTS {0, 0, 0, 0, 0}	// IPARK初始化参数

// ==================== 克拉克变换常数定义 ====================
#define CLARKE_COEF_1  (1.0f)
#define CLARKE_COEF_2  (-0.5f)
#define CLARKE_COEF_3  (0.8660254f)  // √3/2 ≈ 0.86602

// ==================== 逆克拉克变换常数定义 ====================
#define INV_CLARKE_COEF_1  (1.0f)        // α系数（Iu）
#define INV_CLARKE_COEF_2  (-0.5f)       // α系数（Iv/Iw）
#define INV_CLARKE_COEF_3  (0.8660254f)  // √3/2（β系数，Iv）
#define INV_CLARKE_COEF_4  (-0.8660254f) // -√3/2（β系数，Iw）


typedef struct 
{
	float Ud; // 二相静止坐标系 d 轴电压
	float Uq;	// 二相静止坐标系 q 轴电压
	float Theta;  // 电机目标电角度,用于FOC计算
	float M_Theta;// 电机目标角度，用于PID计算的目标值存储
	float Speed;  // 电机目标转速（转/分钟）
	float Umax;		// 最大电压幅值（用于SPWM计算）
} SPWM, *M_SPWM;

#define SPWM_DEFAULTS {0, 0, 0, 0,0 ,12.0f} // 初始SPWM化参数


typedef struct 
{
	float Ud; // 二相静止坐标系 d 轴电压
	float Uq;	// 二相静止坐标系 q 轴电压
	float Theta;  // 电机目标电角度,用于FOC计算
	float M_Theta;// 电机目标角度，用于PID计算的目标值存储
	float Speed;  // 电机目标转速（转/分钟）
	float Umax;		// 最大电压幅值（用于SPWM计算）
} SVPWM, *M_SVPWM;

#define SVPWM_DEFAULTS {0, 0, 0, 0, 0, 12.0f} // 初始SVPWM化参数

void SPWM_Calc(M_SPWM pv);
void SVPWM_Calc(M_SVPWM pv);
extern SPWM spwm;
extern SVPWM svpwm;
extern IPARK ipark;
extern ICLARKE iclarke;
extern CLARKE clarke;
extern PARK park;
void ClarkeCale(M_CLARKE pv);
void ParkCale(M_PARK pv);

#endif
