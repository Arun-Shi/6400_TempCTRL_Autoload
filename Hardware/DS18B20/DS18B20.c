#include "./Base_Head_File.h"

//DS18B20驱动，包括初始化，开始转换，读取Rom，读取温度等操作，其中ALL相关操作是同时进行多引脚操作，需要将单总线上拉至VCC		不要使用

DS_Struct DS_ALL_Arr[__MAX_DS_NUM]={
	{TRUE,D5,0.0},{TRUE,D6,0.0},{TRUE,D7,0.0},{TRUE,G9,0.0},{TRUE,G13,0.0},{TRUE,G14,0.0},{TRUE,G15,0.0},{TRUE,B4,0.0},{TRUE,B5,0.0},{TRUE,B6,0.0}
};
/*******************************多DS18B20操作***************************/
void DS18B20_Init(void)
{
	for(u8 index=0; index< __MAX_DS_NUM; index++)
		SmpIO_Init(DS_ALL_Arr[index].Pin, OD_1);
	
	DS18B20_ALL_Start_Convert();
}
//所有DS18B20进行状态重置
u8 DS18B20_ALL_Reset(void)
{
	u8 DS_Status=0;
	taskENTER_CRITICAL();
	__DS_ALL_Pin_Hight
	__DS_ALL_Pin_Low
	Delay_us(470);
	__DS_ALL_Pin_Hight
	Delay_us(60);
	
	__DS_ALL_Pin_Get(DS_Status)
	
	Delay_us(470);
	__DS_ALL_Pin_Hight
	taskEXIT_CRITICAL();
 	return DS_Status;
}

//所有DS18B20发送一字节数据
void DS18B20_ALL_Send_Byte(u8 data)
{
	taskENTER_CRITICAL();
	for(u8 i=0;i<8;i++)
	{
		__DS_ALL_Pin_Low
		Delay_us(12);
		if(data & 1)
			__DS_ALL_Pin_Hight
		else
			__DS_ALL_Pin_Low
		
		data >>=1;
		
		Delay_us(60);
		__DS_ALL_Pin_Hight
	}
	taskEXIT_CRITICAL();
}
void DS18B20_ALL_Read_Byte(u8* Buff)
{
	u8 data[__MAX_DS_NUM] ={0};
//逐个读取耗时7ms
	taskENTER_CRITICAL();
	for(u8 i=0; i<__MAX_DS_NUM; i++)
		data[i]=DS18B20_Read_Byte(DS_ALL_Arr[i].Pin);
	memmove(Buff,data,__MAX_DS_NUM);
	taskEXIT_CRITICAL();
}
u8 DS18B20_ALL_Start_Convert(void)
{
	u8 DS_status=TRUE;
	
	DS_status= DS18B20_ALL_Reset();		//复位
	DS18B20_ALL_Send_Byte(0XCC);		//跳过ROM检测
	DS18B20_ALL_Send_Byte(0X44);		//启动温度转换
	
	return DS_status;
}
void DS18B20_ALL_Read_Temp_float(void)
{
	u8 Data_Low[__MAX_DS_NUM]={0}, Data_hight[__MAX_DS_NUM]={0};
	u16 data[__MAX_DS_NUM]={0};
	u8 j=0;
	
	DS18B20_ALL_Reset();					//复位
	DS18B20_ALL_Send_Byte(0XCC);      		//跳过ROM检测
	DS18B20_ALL_Send_Byte(0XBE);      		//读取暂存器指令
	
	DS18B20_ALL_Read_Byte(Data_Low);		//读温度低位
	DS18B20_ALL_Read_Byte(Data_hight);		//读温度高位

	for(j=0; j< __MAX_DS_NUM; j++)
		data[j]=(Data_hight[j]<< 8 )+ Data_Low[j];

	for(j=0;j<__MAX_DS_NUM;j++)
	{
//读取异常时重新读取当前位，直到正常或者错误超5次(此时赋值)
		static u8 read_cycle=0;
		RE_JUDGE:
		if(data[j]==0xFFFF || (DS_ALL_Arr[j].Tempdata!=0 && (fabs(data[j]* 0.0625 - DS_ALL_Arr[j].Tempdata)>10)))	//某些情况下，读取温度将会失败，表现为所有位为1或者温度体跳变大于10摄氏度（每秒一次检测），此时重新读取一次
		{
			if(read_cycle<5)
			{
				read_cycle++;
				DS18B20_Reset(DS_ALL_Arr[j].Pin);
				DS18B20_Send_Byte(DS_ALL_Arr[j].Pin, 0XCC);      		//跳过ROM检测
				DS18B20_Send_Byte(DS_ALL_Arr[j].Pin, 0XBE);      		//读取暂存器指令
				data[j]= (DS18B20_Read_Byte(DS_ALL_Arr[j].Pin)<<8)+ DS18B20_Read_Byte(DS_ALL_Arr[j].Pin);
				goto RE_JUDGE;
			}
			
		}
		read_cycle=0;
		if(data[j]&0X800)					//如果是负数，则将补码还原成原码
		{
			data[j]=~data[j]+1;
			DS_ALL_Arr[j].Tempdata= -(data[j] * 0.0625);
		}
		else
			DS_ALL_Arr[j].Tempdata= data[j] * 0.0625;
	}
}

/*****************************单DS18B20操作*************************** */
//指定某一DS18B20进行状态重置
#if 0
u8 DS18B20_Reset(IO_Pos Pin)
{

	u8 DS_Status=1;

	DS_Pin_Hight(Pin);
	Delay_us(2);
	DS_Pin_Low(Pin);
	Delay_us(480);
	DS_Pin_Hight(Pin);
	Delay_us(60);
	
	DS_Status=DS_Pin_Get(Pin);
	
	Delay_us(480);
	DS_Pin_Hight(Pin);
	
	return DS_Status;

}
//指定某一DS18B20发送一字节数据
void DS18B20_Send_Byte(IO_Pos Pin, u8 data)
{
	for(u8 i=0;i<8;i++)
	{
		DS_Pin_Low(Pin);
		Delay_us(2);
		
		if(data & 1)
			DS_Pin_Hight(Pin);
		else
			DS_Pin_Low(Pin);
		
		data >>=1;
		
		Delay_us(60);
		DS_Pin_Hight(Pin);
		
		Delay_us(2);
	}
}


u8 DS18B20_Read_Byte(IO_Pos Pin)
{
	u8 data =0;
	static u8 a=2,b=12,c=60;

	for(u8 i=0;i<8;i++)
	{
		data>>=1;
		DS_Pin_Low(Pin);
		Delay_us(a);
		
		DS_Pin_Hight(Pin);
		Delay_us(b);
		if(DS_Pin_Get(Pin))
			data|=0x80;
		Delay_us(c);
	}

	return data;
}

u8 DS18B20_Start_Convert(IO_Pos Pin)
{
	u8 DS_status=TRUE;
	
	DS_status&= DS18B20_Reset(Pin);		//复位
	DS18B20_Send_Byte(Pin,0XCC);		//跳过ROM检测
	DS18B20_Send_Byte(Pin,0X44);		//启动温度转换
	
	return DS_status;
}

float DS18B20_Read_Temp_float(IO_Pos Pin)		//读取温度
{
	u8 Data_Low,Data_hight;
	u16 data;
	
	DS18B20_Reset(Pin);					//复位
	DS18B20_Send_Byte(Pin, 0XCC);      	//跳过ROM检测
	DS18B20_Send_Byte(Pin, 0XBE);      	//读取暂存器指令
	
	Data_Low=DS18B20_Read_Byte(Pin);	//读温度低位
	Data_hight=DS18B20_Read_Byte(Pin);	//读温度高位
	
	data=(Data_hight<<8)+Data_Low;

	if(data&0X800)						//如果是负数，则将补码还原成原码
	{
		data=~data+1;
		return -(data * 0.0625);
	}
	else
		return data * 0.0625;
}

int DS18B20_Read_Temp_int(IO_Pos Pin)
{
	u8 Data_Low,Data_hight;
	
	DS18B20_Reset(Pin);					//复位
	DS18B20_Send_Byte(Pin, 0XCC);      	//跳过ROM检测
	DS18B20_Send_Byte(Pin, 0XBE);      	//读取暂存器指令
	
	Data_Low=DS18B20_Read_Byte(Pin);	//读温度低位
	Data_hight=DS18B20_Read_Byte(Pin);	//读温度高位
	
	return (Data_hight<<8)+Data_Low;
}
#endif
