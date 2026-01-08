#include "./usart/usart.h" 

Serial_Max UART1_RX;
u8 Cge485_T=3;
#ifdef USE_PRINT_VAR
/*
函	数：选择通道打印内容
参	数：Ch-	通道，指的是串口通道
		后接可变参数，直接替换到printf函数的参数中
*/
void Printf_Chx(USART_TypeDef* USARTx, const char* format, ...)
{
	static char Buff[512];
	xSemaphoreTake(xMutex_UartTX, portMAX_DELAY);								//获取打印信号量
	va_list ap;	
    va_start(ap, format);
    int length = vsnprintf(Buff, sizeof(Buff), format, ap);
    va_end(ap);
	
	if (length < 0)																//处理格式化错误，例如记录或返回
		goto end;
	length = (length >= (int)sizeof(Buff)) ? (int)sizeof(Buff) - 1 : length;	//确保长度不超过缓冲区容量
	
	for (int j = 0; j < length; j++)
	{
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) continue;	//等待发送寄存器就绪
			USART_SendData(USARTx, (uint8_t)Buff[j]);
	}
	end:
	xSemaphoreGive(xMutex_UartTX);												//释放打印信号量
}
#else
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#pragma import(__use_no_semihosting)
USART_TypeDef* Select_P_CH;
        
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
	
	while((Select_P_CH->SR&0X40)==0);//循环发送,直到发送完毕
		Select_P_CH->DR = (char) ch;

	return ch;
}

void _ttywrch(int ch)
{
	ch=ch;
}
#endif
//发送一个字节
void UARTX_SendByte(USART_TypeDef * USARTx, char Char)
{
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) continue;        // 等待发送寄存器就绪
		USART_SendData(USARTx, Char);
}
//将内存空间中的一部分整体发送
void Printf_Mem_Chx(USART_TypeDef *USARTx, void* Addr, u16 Len)
{
	u32 i=0;
	while(i<Len)
	{
		while (!(USARTx->SR & USART_SR_TXE)) continue;	// 等待发送缓冲区为空
			USARTx->DR = ((char*)Addr)[i++];	// 发送一个字节
	}
}

void USARTx_Init(USART_TypeDef* UARTx,u32 Bound)
{
//串口外设参数设置
	USART_InitTypeDef USART_InitStructure={
		.USART_BaudRate = Bound,
		.USART_WordLength = USART_WordLength_8b,
		.USART_StopBits = USART_StopBits_1,
		.USART_Parity = USART_Parity_No,
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
		.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
	};
//NVIC参数设置
	NVIC_InitTypeDef NVIC_InitStructure={
		.NVIC_IRQChannelPreemptionPriority=3,	//抢占优先级
		.NVIC_IRQChannelSubPriority = 3,		//子优先级
		.NVIC_IRQChannelCmd = ENABLE,			//IRQ通道使能
	};
//据通道初始化
	if(UARTx==USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		SmpIO_Init(A9,AF_PP);		//Tx
		SmpIO_Init(A10,IF);			//Rx
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	}
	else if(UARTx==USART2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
		SmpIO_Init(A2,AF_PP);		//Tx
		SmpIO_Init(A3,IF);			//Rx		
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	}
	else if(UARTx==USART3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3时钟
		SmpIO_Init(B10,AF_PP);		//Tx
		SmpIO_Init(B11,IF);			//Rx
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	}
	else if(UARTx==UART4)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//使能USART4时钟
		SmpIO_Init(C10,AF_PP);		//Tx
		SmpIO_Init(C11,IF);			//Rx
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	}
	else if(UARTx==UART5)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//使能USART5时钟
		SmpIO_Init(C12,AF_PP);		//Tx
		SmpIO_Init(D2,IF);			//Rx
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	}
//据参数初始化
	USART_Init(UARTx, &USART_InitStructure);
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(UARTx, USART_IT_RXNE, ENABLE);
	USART_Cmd(UARTx, ENABLE);
}
/************************************************************串口中断函数*********************************************/
#if 1
void USART1_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)		//判断标志位
	{
		ChSW= Ch1;
		uint8_t RxData = USART_ReceiveData(USART1);
		UART1_RX.RXPacket[UART1_RX.len] = RxData;
		UART1_RX.len++;
		
		if(RxData=='\n')
		{
			UART1_RX.RXPacket[UART1_RX.len-2] = '\0';		//为字符串添加结束符
			xSemaphoreGiveFromISR(xSem_UartRx, NULL);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
	USART_ClearITPendingBit(USART1,USART_FLAG_RXNE);
}
void USART2_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)		//判断标志位
	{
		ChSW= Ch2;
		uint8_t RxData = USART_ReceiveData(USART2);
		UART1_RX.RXPacket[UART1_RX.len] = RxData;
		UART1_RX.len++;
		
		if(RxData=='\n')
		{
			UART1_RX.RXPacket[UART1_RX.len-2] = '\0';		//为字符串添加结束符
			xSemaphoreGiveFromISR(xSem_UartRx, NULL);		//置标志位
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
	USART_ClearITPendingBit(USART2,USART_FLAG_RXNE);
}
void USART3_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)		//判断标志位
	{
		ChSW= Ch3;
		uint8_t RxData = USART_ReceiveData(USART3);
		UART1_RX.RXPacket[UART1_RX.len] = RxData;
		UART1_RX.len++;
		
		if(RxData=='\n')
		{
			UART1_RX.RXPacket[UART1_RX.len-2] = '\0';		//为字符串添加结束符
			xSemaphoreGiveFromISR(xSem_UartRx, &xHigherPriorityTaskWoken);		//置标志位
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
	USART_ClearITPendingBit(USART3,USART_FLAG_RXNE);
}
#endif

