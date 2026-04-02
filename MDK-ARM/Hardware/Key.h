#ifndef _KEY_H_
#define _KEY_H_

#define KEY_NUM_MAX 4   //最大按键数量

typedef struct
{
    GPIO_TypeDef *gpio;
	uint32_t pin;
}Key_List_t;

typedef enum
{
	Key_Release = 0,         // 释放松开
	Key_Confirm = 1,             // 消抖确认
	Key_ShortPress = 2,          // 短按
	Key_LongPress = 3            // 长按
} Key_State;

typedef struct
{
    Key_State keyState; // 按键状态
	uint8_t SingleClickNum;// 单击次数计数器
	uint64_t FirstIoChangeSysTime;// 第一次IO状态改变的时间戳
	uint64_t FirstReleaseSysTime;// 第一次松开的时间戳
}Key_Info_t;


#define CONFIRM_TIME                 10       // 按键消抖时间窗10ms
#define DOUBLE_CLICK_INTERVAL_TIME   300      // 两次抬起时间窗300ms，用来判断是否双击
#define LONGPRESS_TIME               2000     // 长按时间窗2000ms

#define KEY1_SHORT_PRESS   0X01
#define KEY1_DOUBLE_PRESS  0X51
#define KEY1_LONG_PRESS    0X81
#define KEY2_SHORT_PRESS   0X02
#define KEY2_DOUBLE_PRESS  0X52
#define KEY2_LONG_PRESS    0X82
#define KEY3_SHORT_PRESS   0X03
#define KEY3_DOUBLE_PRESS  0X53
#define KEY3_LONG_PRESS    0X83
#define KEY4_SHORT_PRESS   0X04
#define KEY4_DOUBLE_PRESS  0X54
#define KEY4_LONG_PRESS    0X84

void Key_Init(void);
uint8_t Key_NumGet(void);

#endif
