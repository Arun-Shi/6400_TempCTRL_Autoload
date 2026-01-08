#ifndef __DS18B20_H
#define __DS18B20_H


typedef enum _DS_CMD{				//DS18B20指令表
	Read_Rom=			0x33,		//读取64位Rom
	APNT_Rom=			0x55,		//指定Rom
	Skip_Rom=			0xCC,		//跳过Rom检测
	Alarm_Get=			0xEC,		//获取警报器件
	Serch_Rom=			0xF0,		//搜索Rom
	
	Start_CNVT= 		0x44,		//开启温度转换
	Read_Temp= 			0xBE,		//读取温度寄存器
	Set_Limit= 			0x4E,		//设置温度阈值
	Copy_Temp_Rom=		0x48,		//复制温度寄存器到EEprom
	
	Read_Battery=		0xB4,		//读取供电方式0为寄生供电，1为外接供电
}DS_CMD;

typedef struct _DS_Sruct{
	__Status STA;
	u8 Pin;
	float Tempdata;
}DS_Struct;

#define __MAX_DS_NUM 10

#define DS_Pin_Hight(Pin)				(SmpIO_Set(Pin,TRUE))

#define DS_Pin_Low(Pin)					(SmpIO_Set(Pin,FALSE))

#define DS_Pin_Get(Pin)					(SmpIO_Read(Pin))

#define __DS_ALL_Pin_Hight				{do{u8 index=0;	for(; index< __MAX_DS_NUM; index++)\
															if(DS_ALL_Arr[index].STA == TRUE)\
																SmpIO_Set(DS_ALL_Arr[index].Pin,TRUE);}\
										while(0);}

#define __DS_ALL_Pin_Low				{do{u8 index=0;	for(; index< __MAX_DS_NUM; index++)\
															if(DS_ALL_Arr[index].STA == TRUE)\
																SmpIO_Set(DS_ALL_Arr[index].Pin,FALSE);}\
										while(0);}

#define __DS_ALL_Pin_Get(Buff)			{do{u8 index=0;	for(; index< __MAX_DS_NUM; index++)\
															if(DS_ALL_Arr[index].STA == TRUE)\
																Buff|=(SmpIO_Read(DS_ALL_Arr[index].Pin)<<index);}\
										while(0);}			//将所有_DS的引脚读取状态存入到Buff中，Buff为一个整数型变量，不能用立即数
extern DS_Struct DS_ALL_Arr[__MAX_DS_NUM];

										
u8 DS18B20_Add_Struct(u8 Pin);
__Status DS18B20_Delete_Struct(u8 Index);
__Status DS18B20_Change_Status(u8 Index,__Status Status);
void DS18B20_Init(void);
u8 DS18B20_ALL_Reset(void);
u8 DS18B20_Reset(u8 Pin);
void DS18B20_Send_Byte(u8 Pin, u8 data);
void DS18B20_ALL_Send_Byte(u8 data);
u8 DS18B20_Read_Byte(u8 Pin);
void DS18B20_ALL_Read_Byte(u8* Buff);
u8 DS18B20_Start_Convert(u8 Pin);
u8 DS18B20_ALL_Start_Convert(void);
float DS18B20_Read_Temp_float(u8 Pin);		//读取温度
void DS18B20_ALL_Read_Temp_float(void);
int DS18B20_Read_Temp_int(u8 Pin);

#endif
