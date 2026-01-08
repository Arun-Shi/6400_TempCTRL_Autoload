#ifndef __USART_H__
#define __USART_H__
#include "./Base_Head_File.h"

#define	Ch1 		USART1
#define	Ch2 		USART2
#define	Ch3 		USART3
#define	Ch4 		UART4
#define	Ch5 		UART5
#define W50			NULL

	#define USE_PRINT_VAR			//指定如何实现Printf_Chx
	
#ifdef USE_PRINT_VAR
	#define T485_Rmod					do{Delay_us(Cge485_T);SmpIO_Set(C4,0);SmpIO_Set(C5,0);	}while(0)									//485模块控制，0发1收
	#define T485_Tmod					do{SmpIO_Set(C4,1);SmpIO_Set(C5,1);Delay_us(Cge485_T);	}while(0)									//485模块控制，0发1收

		void Printf_Chx(USART_TypeDef * USARTx, const char * format, ...);				//指定串口输出字符串，字符串格式同printf
#else
	extern USART_TypeDef* Select_P_CH;
	#define Printf_Chx(USARTx, format, ...) 	do{Select_P_CH = USARTx;	printf(format, ##__VA_ARGS__);}while(0)
#endif
typedef struct
{
	u16 len;
	uint8_t RXFlag;
	char RXPacket[1024];
}Serial_Max;
typedef struct
{
	u16 len;
	uint8_t RXFlag;
	char RXPacket[128];
}Serial_Min;

void USARTx_Init(USART_TypeDef * UARTXx,u32 Bound);								//指定串口，按波特率参数初始化
void UARTX_SendByte(USART_TypeDef * USARTx,char Char);							//指定串口输出一个字节
void Printf_Mem_Chx(USART_TypeDef *USARTx, void* Addr, u16 Len);				//指定串口输出一整段内存

extern Serial_Max UART1_RX;

#endif
