#include "./BSP_IIC/IIC.H"
//IO操作函数	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA
#define READ_SDA   PBin(7)  //输入SDA

#define IIC_Freq	800		//单位khz,500Khz以下时，使用自动计算延时时间
// #define IIC_Delay()	Delay_us(1.0/IIC_Freq*1000/2)				//自动计算延时
#define IIC_Delay()		Delay_500ns()								//手动配置延时

//初始化IIC
void MyIIC_Init(void)
{
	SmpIO_Init(B6,PP_1);			//SCL
	SmpIO_Init(B7,OD_1);			//SDA
}
//产生IIC起始信号
void IIC_Start(void)
{
	IIC_SDA=1;
	IIC_SCL=1;
	IIC_Delay();
 	IIC_SDA=0;
	IIC_Delay();
	IIC_SCL=0;		//钳住IIC总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	IIC_SDA=0;
	IIC_SCL=1; 
	IIC_Delay();
	IIC_SDA=1;		//发送IIC总线结束信号
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	IIC_SDA=1;
	IIC_Delay();
	IIC_SCL=1;
	IIC_Delay();
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>50)
		{
			IIC_Stop();
			return 1;
		}
		IIC_Delay();
	}
	IIC_SCL=0;
	return 0;  
} 
void IIC_SendAck(u8 Ack)
{
	IIC_SCL=0;
	IIC_SDA=!Ack;
	IIC_Delay();
	IIC_SCL=1;
	IIC_Delay();
	IIC_SCL=0;
	IIC_SDA= 1;     //释放SDA线
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		IIC_Delay();
		IIC_SCL=1;
		IIC_Delay();
		IIC_SCL=0;	
		IIC_Delay();
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        IIC_Delay();
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		IIC_Delay();
    }
    IIC_SendAck(ack);
    return receive;
}

void Scan_IIC(void)
{
	u8 temp;
    for(u8 addr=0; addr<= 0x7F; addr++)
	{
		temp=addr<<1;
        if(Addr_Chek(temp))
		{
            Delay_ms(1);
            if(Addr_Chek(temp))				//双重验证
				Printf_Chx(ChSW,"%02X ",temp);
		}
	}
}
u8 Addr_Chek(u8 Addr)
{
	u8 status=1;
	xSemaphoreTake(xMutex_IICBus, 10);
    taskENTER_CRITICAL();					// 开启时序保护
	IIC_Start();  
	IIC_Send_Byte(Addr);	   				// 发送写命令
	if(IIC_Wait_Ack())						// 应答失败
		status= 0;
	IIC_Stop();
	taskEXIT_CRITICAL();					// 释放保护
	xSemaphoreGive(xMutex_IICBus);
	Delay_us(10);
	
	return status;
}
/**
 * @brief  通用写事务
 * @param  DevAddr:  从机地址(写)
 * @param  RegAddr:  寄存器地址
 * @param  RegLen:   寄存器地址长度 (0:无地址, 1:8位地址, 2:16位地址)
 * @param  pData:    待发送数据指针
 * @param  DataLen:  发送数据长度
 */
u8 IIC_Write_Ex(u8 DevAddr, u16 RegAddr, u8 RegLen, u8* pData, u16 DataLen)
{
    u8 status = 0;
	xSemaphoreTake(xMutex_IICBus, 10);
    taskENTER_CRITICAL(); // 开启时序保护

    IIC_Start();
    IIC_Send_Byte(DevAddr);
    if(IIC_Wait_Ack()) { status = 1; goto end; }

    // 发送寄存器地址 (处理 8bit/16bit)
    if(RegLen == 2) {
        IIC_Send_Byte((u8)(RegAddr >> 8)); // 高位在前
        if(IIC_Wait_Ack()) { status = 2; goto end; }
    }
    if(RegLen >= 1) {
        IIC_Send_Byte((u8)(RegAddr & 0xFF));
        if(IIC_Wait_Ack()) { status = 3; goto end; }
    }

    // 发送数据内容
    for(u16 i=0; i<DataLen; i++) {
        IIC_Send_Byte(pData[i]);
        if(IIC_Wait_Ack()) { status = 4; goto end; }
    }

end:
    IIC_Stop();
    taskEXIT_CRITICAL(); // 释放保护
	xSemaphoreGive(xMutex_IICBus);
    return status;
}
/**
 * @brief  通用读事务
 * @param  RegLen: 寄存器地址长度 (0:直接读, 1:8位地址, 2:16位地址)
 */
u8 IIC_Read_Ex(u8 DevAddr, u16 RegAddr, u8 RegLen, u8* pBuf, u16 DataLen)
{
    u8 status = 0;
	xSemaphoreTake(xMutex_IICBus, 10);
    taskENTER_CRITICAL();

    // 1. 如果有寄存器地址，先执行“伪写”定位地址指针
    if(RegLen > 0) {
        IIC_Start();
        IIC_Send_Byte(DevAddr);
        if(IIC_Wait_Ack()) { status = 1; goto end; }
        
        if(RegLen == 2) {
            IIC_Send_Byte((u8)(RegAddr >> 8));
            if(IIC_Wait_Ack()) { status = 2; goto end; }
        }
        IIC_Send_Byte((u8)(RegAddr & 0xFF));
        if(IIC_Wait_Ack()) { status = 3; goto end; }
    }

    // 2. 切换到读模式
    IIC_Start(); // Re-Start
    IIC_Send_Byte(DevAddr | 1);
    if(IIC_Wait_Ack()) { status = 4; goto end; }

    for(u16 i=0; i<DataLen; i++) {
        pBuf[i] = IIC_Read_Byte((i == (DataLen - 1)) ? 0 : 1);
    }

end:
    IIC_Stop();
    taskEXIT_CRITICAL();
	xSemaphoreGive(xMutex_IICBus);
    return status;
}
/*
函	数：检测IIC总线是否空闲
参	数：
返	回：0- 总线空闲
		1- 总线繁忙
实	现：读取IIC总线的电平，双线高电平则为空闲，循环100次共100us等待，超时则返回1
*/
u8 IIC_Bus_BusyCheck(void)
{
	u8 i=100;
	for(;i>0;i--)
	{
		if(!(SmpIO_Read(B6) & SmpIO_Read(B7)))
		{
			Delay_us(1);
			continue;
		}
		else
			return 0;
		
	}
	return 1;
}
/****************************
说  明：954X选路
参  数：Addr- IIC拓展器件地址
		Line- 选择的路(0、1、2、3、4)
返回值：0- 操作成功		1- 操作失败
备  注：兼容9545一分四和9548一分八
****************************/
u8 PCA954X_Select_Line(u8 Addr, u8 Line)
{
    u8 data = (Line == 0) ? 0 : (1 << (Line - 1));
    u8 status = 0;

    // 1. 检查总线（此时不宜进入临界区，因为可能涉及长等待）
    if(IIC_Bus_BusyCheck()) return 1; 

    // 2. 开启原子保护，从 START 到 STOP 一气呵成
    taskENTER_CRITICAL();
    
    IIC_Start();
    
    IIC_Send_Byte(Addr);
    if(IIC_Wait_Ack()) { status = 2; goto end; }
    
    IIC_Send_Byte(data);
    if(IIC_Wait_Ack()) { status = 3; goto end; }

end:
    IIC_Stop();
    taskEXIT_CRITICAL(); 
    
    return status;
}
