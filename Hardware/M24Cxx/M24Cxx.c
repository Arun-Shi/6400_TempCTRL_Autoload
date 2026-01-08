#include "./M24Cxx/M24Cxx.h"

extern EEP_Size IIC_Chip;
u8 EEP_TYPE= EEP_C02;
//获取当前eep芯片的页尺寸
u16 EEP_PageSize(EEP_Size Chip)
{
	switch(Chip)
	{
    case EEP_C01:
	case EEP_C02:
	  return 8;
	  
	case EEP_C04:
	case EEP_C08:
	case EEP_C16:
	  return 16;
	  
	case EEP_C32:
	case EEP_C64:
      return 32;
        
	case EEP_C128:
	case EEP_C256:
      return 64;

	case EEP_C512:
      return 128;

	case EEP_C1024:
	  return 256;
		  
	default:
	  return 0;
	}
}
u8 EEP_R_1Byte(u8 Devic_Addr, u16 Reg_Addr, u8* Buff)
{
    u8 status=0;
    IIC_Start();
	if(EEP_TYPE > EEP_C16)									//eeprom大容量时，读写地址变化
	{
		IIC_Send_Byte(Devic_Addr);	   						//写指针
		status|=IIC_Wait_Ack();
		IIC_Send_Byte(Reg_Addr>>8);							//发送高地址
	}
	else
		IIC_Send_Byte(Devic_Addr+((Reg_Addr/256)<<1)); 
	status|=IIC_Wait_Ack(); 
	
	IIC_Send_Byte(Reg_Addr%256);   							//发送低地址
	status|=IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(Devic_Addr+1);								//进入接收模式
	status|=IIC_Wait_Ack();
    *Buff=IIC_Read_Byte(FALSE);
    IIC_Stop();
	return status;
}
u8 EEP_R_nByte(u8 Devic_Addr, u16 Reg_Addr, u8* Buff, u32 size)
{
	u8 status=0;
	IIC_Start();
	if(EEP_TYPE > EEP_C16)									//eeprom大容量时，读写地址变化
	{
		IIC_Send_Byte(Devic_Addr);
		status|=IIC_Wait_Ack();
		IIC_Send_Byte(Reg_Addr>>8);							//发送高地址
	}
	else
		IIC_Send_Byte(Devic_Addr+((Reg_Addr/256)<<1)); 
	status|=IIC_Wait_Ack(); 
	
	IIC_Send_Byte(Reg_Addr%256);   							//发送低地址
	status|=IIC_Wait_Ack();
	
	IIC_Start();
	IIC_Send_Byte(Devic_Addr+1);							//进入接收模式
	status|=IIC_Wait_Ack();
	
	while(size--)
		*(Buff++)=IIC_Read_Byte(size? TRUE:FALSE);

	IIC_Stop();
	return status;
}
/*		注意，此函数没有加入器件写周期相关的操作，连续写入时自行加入10ms写周期		*/
u8 EEP_W_1Byte(u8 Devic_Addr, u16 Reg_Addr, u8 Data)
{				  
    u8 status=0;
	IIC_Start();
	if(EEP_TYPE>EEP_C16)										//大于24C16的器件，需要发送2字节地址
	{
		IIC_Send_Byte(Devic_Addr);
		status|=IIC_Wait_Ack();
		
		IIC_Send_Byte(Reg_Addr>>8);
	}
	else 
		IIC_Send_Byte( Devic_Addr+((Reg_Addr/256)<<1));
	status|=IIC_Wait_Ack();
	
	IIC_Send_Byte(Reg_Addr%256);
	status|=IIC_Wait_Ack();
	IIC_Send_Byte(Data);     //发送字节							   
	status|=IIC_Wait_Ack();   		    	   
    IIC_Stop();//产生一个停止条件 
	return status;
}
u8 EEP_W_nByte(u8 Devic_Addr, u16 Reg_Addr, u8* Data, u32 Size)
{
	u8 status=0;										//写入状态
	u32 wr_index=0;										//写入索引，表示已写入的字节数，也表示写入地址的偏移量
	u8 wr_cycle_size=0;									//当前写周期的写入字节数
	u8 STA=0;											//写周期等待时的 状态获取
	u32 STA_Cnt=0;										//等待计数
	u16 Page_Size=EEP_PageSize(EEP_TYPE);
//当满足 当前写地址为页尺寸的整数倍 和 当前剩余字节数大于页尺寸时，可进行页读写
	u8 pageWr_flag= ((Reg_Addr+ wr_index)% Page_Size== 0)&&\
					((Size-wr_index)/ Page_Size!= 0);
	while(Size> wr_index)								//写入大循环，只要写入不超出Size就循环写
	{
//执行写指针操作
		STA_Cnt=0;
		do{
			STA=0;
			IIC_Start();
			if(EEP_TYPE>EEP_C16)								//大于24C16的器件，需要发送2字节地址
			{
				IIC_Send_Byte(Devic_Addr);
				STA|=IIC_Wait_Ack();
				IIC_Send_Byte((Reg_Addr+ wr_index)>>8);
			}
			else 
				IIC_Send_Byte( Devic_Addr+(((Reg_Addr+ wr_index)/256)<<1));
			STA|=IIC_Wait_Ack();
			
			IIC_Send_Byte((Reg_Addr+ wr_index)%256);
			STA|=IIC_Wait_Ack();
			if(STA==1)										//判断响应状态
			{
				STA_Cnt++;
				Delay_us(5);
				if(STA_Cnt>500)
					return 1;
			}
		}while(STA);
//判断页写入标志，给写入字节数赋值
		if(pageWr_flag)
			wr_cycle_size=Page_Size;
		else
			wr_cycle_size=1;
//执行写循环
		for(u8 j=0;j< wr_cycle_size;j++,wr_index++)
		{
			IIC_Send_Byte(*(Data+ wr_index));
			status|=IIC_Wait_Ack();
		}
		IIC_Stop();//产生一个停止条件 
		pageWr_flag=((Reg_Addr+ wr_index)% Page_Size== 0)&& ((Size-wr_index)/ Page_Size!= 0);
	}
	return status;
}
/**
 * @brief  EEPROM 高级写接口（支持跨页写入与RTOS保护）
 * @param  DevAddr: 器件地址（如 0xA0）
 * @param  RegAddr: 寄存器起始地址
 * @param  pData:   待写入数据指针
 * @param  Size:    写入字节长度
 * @return 0:成功, 其他:错误码
 */
u8 EEP_Write_Ex(u8 DevAddr, u16 RegAddr, u8* pData, u32 Size)
{
    u8 status = 0;
    u32 wr_index = 0;
    u16 Page_Size = EEP_PageSize(EEP_TYPE);
    u8 current_DevAddr;
    u16 current_RegAddr;
    u8 reg_len;

    while (wr_index < Size)
    {
        // 1. 计算当前周期可以写入的长度（不能超过页边界，不能超过剩余长度）
        u32 left_in_page = Page_Size - ((RegAddr + wr_index) % Page_Size);
        u32 wr_cycle_size = (Size - wr_index > left_in_page) ? left_in_page : (Size - wr_index);

        // 2. 处理不同型号地址映射
        if (EEP_TYPE > EEP_C16) {
            // 大容量：双字节地址
            current_DevAddr = DevAddr;
            current_RegAddr = RegAddr + wr_index;
            reg_len = 2;
        } else {
            // 小容量：地址高位在 DevAddr 中，低位在 RegAddr
            current_DevAddr = DevAddr + ((((RegAddr + wr_index) / 256) << 1));
            current_RegAddr = (RegAddr + wr_index) % 256;
            reg_len = 1;
        }

        // 3. 调用通用接口写入（内部自带信号量和临界区）
        // 这里采用轮询(Polling)机制：如果芯片正在内部写入，IIC_Write_Ex 会因为收不到 ACK 而返回非0
        u16 retry = 50; 
        do {
            status = IIC_Write_Ex(current_DevAddr, current_RegAddr, reg_len, &pData[wr_index], wr_cycle_size);
            if (status != 0) vTaskDelay(1); // 没响应则稍作等待
        } while (status != 0 && --retry);

        if (retry == 0) return 1; // 写入超时

        wr_index += wr_cycle_size;
    }
    return 0;
}
/**
 * @brief  EEPROM 高级读接口
 * @param  DevAddr: 器件地址（如 0xA0）
 * @param  RegAddr: 寄存器起始地址
 * @param  pData:   读出数据缓存地址指针
 * @note   读取通常没有页限制，可以连续读取
 */
u8 EEP_Read_Ex(u8 DevAddr, u16 RegAddr, u8* pBuf, u32 Size)
{
    u8 current_DevAddr;
    u16 current_RegAddr;
    u8 reg_len;

    //处理 24C16 的分页地址问题
    if (EEP_TYPE > EEP_C16) {
        current_DevAddr = DevAddr;
        current_RegAddr = RegAddr;
        reg_len = 2;
    } else {
        current_DevAddr = DevAddr + (((RegAddr / 256) << 1));
        current_RegAddr = RegAddr % 256;
        reg_len = 1;
    }

    // 调用通用读接口（内部自带信号量和临界区）
    return IIC_Read_Ex(current_DevAddr, current_RegAddr, reg_len, pBuf, Size);
}
