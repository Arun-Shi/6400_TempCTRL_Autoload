#ifndef __SMP_IO_H__
#define __SMP_IO_H__

#include "./Base_Head_File.h"

//位带操作,实现51类似的GPIO控制功能
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr & 0xFFFFF)<<5)+(bitnum << 2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,(n))  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,(n))  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,(n))  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,(n))  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,(n))  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,(n))  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,(n))  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,(n))  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,(n))  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,(n))  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,(n))  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,(n))  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,(n))  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,(n))  //输入

//精简通用位带操作宏
#define PORT_OFFSET  0x400 						// 定义端口间距 (F1系列通常是 0x400)
#define BITOpe_W	12
#define BITOpe_R	8
//传入端口号(0-6)和寄存器偏移(8=IDR, 12=ODR)，得到寄存器原始地址
#define PORT_REG_ADDR(port_idx, reg_off)  (GPIOA_BASE + (port_idx) * PORT_OFFSET + (reg_off))
#define PX_BIT(idx, n, off)  BIT_ADDR(PORT_REG_ADDR(idx, off), n)	// idx: 端口号, n: 引脚号, off: 寄存器偏移

extern GPIOSpeed_TypeDef DE_SPEED;

typedef enum
{
	A0,	A1,	A2,	A3,	A4,	A5,	A6,	A7,	A8,	A9,	A10,	A11,	A12,	A13,	A14,	A15,
	B0,	B1,	B2,	B3,	B4,	B5,	B6,	B7,	B8,	B9,	B10,	B11,	B12,	B13,	B14,	B15,
	C0,	C1,	C2,	C3,	C4,	C5,	C6,	C7,	C8,	C9,	C10,	C11,	C12,	C13,	C14,	C15,
	D0,	D1,	D2,	D3,	D4,	D5,	D6,	D7,	D8,	D9,	D10,	D11,	D12,	D13,	D14,	D15,
	E0,	E1,	E2,	E3,	E4,	E5,	E6,	E7,	E8,	E9,	E10,	E11,	E12,	E13,	E14,	E15,
	F0,	F1,	F2,	F3,	F4,	F5,	F6,	F7,	F8,	F9,	F10,	F11,	F12,	F13,	F14,	F15,
	G0,	G1,	G2,	G3,	G4,	G5,	G6,	G7,	G8,	G9,	G10,	G11,	G12,	G13,	G14,	G15,
}IO_Pos;

typedef enum
{
	IPU, IPD, IF, AIN, PP_0, PP_1, OD_0, OD_1, AF_PP,AF_OD,
}Mode_TypeDef;

typedef enum
{
	A_Port,B_Port,C_Port,D_Port,E_Port,F_Port,G_Port
}Port_TypeDef;

typedef enum
{
	Out_None, Out_1, Out_0
}Out_TypeDef;

u8 SmpIO_Init(u8 Port_Pin, Mode_TypeDef Mode);
u8 IOexp_Init(Mode_TypeDef Mode);
u8 IOexp_Set(u8 Lev);
u8 SmpIO_Read(u8 Port_Pin);
u8 SmpIO_Set(u8 Port_Pin, u8 Level);
void SmpIO_Toggle(u8 Port_Pin);
u8 SmpIO_StrtPin(char* Str);



/**************************针对项目的关键定义**********************************/
typedef enum __16IO_Trans{
	X1=B8,	X2=B9,	X3=C13,	X4=C14,	X5=C15,	X6=C0,	X7=C1,	X8=C2,	X9=C3,	X10=A0,	X11=A1,	X12=A2,	X13=A3,	X14=A4,	X15=A5,	X16=A6,	
	Y1=B4,	Y2=B3,	Y3=D2,	Y4=C12,	Y5=C11,	Y6=C10,	Y7=A15,	Y8=A12,	Y9=A8,	Y10=C9,	Y11=C8,	Y12=B1,	Y13=B0,	Y14=C5,	Y15=C4,	Y16=A7,	
}__16IO_Trans;

#endif

