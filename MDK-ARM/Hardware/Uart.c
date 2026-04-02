#include "main.h"
#include "usart.h"
#include <stdio.h>

char Vofa_Rx_Data[100];//数据包接收数组
uint8_t Vofa_Rx_Flag;


void UATR_Init(void)
{
	
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	__HAL_UART_CLEAR_FLAG(&huart1,UART_IT_RXNE);//清除RXNE标志位 
	
}

uint8_t GET_Vofa_Rx_Flag(void)
{
	if(Vofa_Rx_Flag==1)
	{
		Vofa_Rx_Flag=0;
		return 1;
	}
	return 0;
}

	
void UATR_RXNE_Handler(void)
{
	static uint8_t State_Flag=0;
	static uint16_t i;
	
	if(__HAL_UART_GET_FLAG(&huart1,UART_IT_RXNE)==RESET)//查询之后会自动清除标志位
	{
		uint8_t data;
		HAL_UART_Receive(&huart1,&data,1, HAL_MAX_DELAY);
		
		switch(State_Flag)//状态机接收数据包
		{
			case 0:
				if(data=='[')//接收到包头
				{
					i=0;
					State_Flag=1;
				}
				break;
				
			case 1:
				if(data==']')//接收到包尾
				{
					State_Flag=0;
					Vofa_Rx_Data[i]='\0';//手动添加字符串结尾
					Vofa_Rx_Flag=1;//接收工作完成
				}
				else//实际数据接收
				{
					Vofa_Rx_Data[i]=data;
					i++;
				}
				break;
		}
		
		__HAL_UART_CLEAR_FLAG(&huart1,UART_IT_RXNE);
	}
}

int fputc(int ch, FILE *f) {
   HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
   return ch;
}

int fgetc(FILE *f) {
   uint8_t ch;
   HAL_UART_Receive(&huart1, &ch, 1, HAL_MAX_DELAY);
   return ch;
}
