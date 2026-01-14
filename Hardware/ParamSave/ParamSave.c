#include "./Base_Head_File.h"

#define _CRC_Value	0x07
#define ParamSave_DevAddr		0xA0    		//M24C02 写地址
#define ParamSave_MagicNum		0x55AA  		//魔数，用于识别是否初始化过
#define ParamSave_Version   	0x0100  		//当前固件参数版本号
#define ParamSave_Ram			NULL
#define ParamSave_RamSize		0

u8 ParamSave_Load_pack(void)
{
	Printf_Chx(ChSW,"Param_Load ...\r\n");
	u8 ope_sta= ParamSave_Load(ParamSave_Ram);
	switch(ope_sta)
	{
		case 0:
			Printf_Chx(ChSW, "[Success]: Parameters loaded and applied.\r\n");
			break;
		case 1:
			Printf_Chx(ChSW, "[Error 1]: EEPROM Read Failed (IIC Hardware Issue).\r\n");
			break;
		case 2:
			Printf_Chx(ChSW, "[Error 2]: Magic Number Mismatch (Virgin Chip?).\r\n");
			break;
		case 3:
			Printf_Chx(ChSW, "[Error 3]: Version Mismatch (Config Structure Changed).\r\n");
			break;
		case 4:
			Printf_Chx(ChSW, "[Error 4]: CRC Checksum Error (Data Corrupted).\r\n");
			break;
		default:
			Printf_Chx(ChSW, "[Unknown]: System Error with code: %d\r\n", ope_sta);
			break;
	};
	return ope_sta;
}
/**
 * @brief  从EEPROM加载参数
 * @return 0:加载成功, 1:数据无效/版本不符(需Reset)
 */
u8 ParamSave_Load(void* DataBuff)
{
    ParamSave_T* load_pkg = (ParamSave_T*)DataBuff;
	u16 crc_len=(u8*)&(load_pkg->Crc_Num) - (u8*)load_pkg;
    u8 crc_calc = 0;
	u8 sta=1;

    //1. 读取整个结构体
    if(EEP_Read_Ex(ParamSave_DevAddr, 0x00, (u8*)load_pkg, sizeof(ParamSave_T)) != 0) 
         goto end; // 硬件读取失败
	sta+=1;
 
    //2. 验证魔数(判断是否是空片或非法数据)
    if(load_pkg->Magic_Num != ParamSave_MagicNum) goto end;	
	sta+=1;

    //3. 验证版本号(判断是否需要进行参数迁移或重置)
    if(load_pkg->Sys_Version != ParamSave_Version) goto end;
	sta+=1;

    //4. 验证校验和
	crc_calc= CRC_Compute8((u8*)load_pkg, crc_len);
    if(crc_calc != load_pkg->Crc_Num) goto end;
	sta+=1;

    //5. 验证通过，分发数据回业务数组
//	memmove()
	sta=0;
	end:
    return sta;				//返回结果
}
/**
 * @brief  保存系统参数至EEPROM
 * @return 0:成功, 1:失败
 */

u8 ParamSave_Save(void* DataBuff)
{
	ParamSave_T* save_pkg =(ParamSave_T*)DataBuff;
	u16 crc_len=(u8*)&(save_pkg->Crc_Num) - (u8*)save_pkg;
	u8 sta=0, err_cycle=5;
    // 1. 聚合数据：从业务数组提取 Config
	memmove(save_pkg->Data, DataBuff, sizeof(ParamSave_T));

    // 2. 填充元数据
    save_pkg->Sys_Version = ParamSave_Version;
    save_pkg->Magic_Num   = ParamSave_MagicNum;

    // 3. 计算CRC校验和 (计算除Crc_Num外所有字节)
	save_pkg->Crc_Num= CRC_Compute8((u8*)save_pkg, crc_len);

    // 4. 写入判断
    sta= EEP_Write_Ex(ParamSave_DevAddr, 0x00, (u8*)save_pkg, sizeof(ParamSave_T));
	
	for(u8 i=0;i< err_cycle;i++)
	{
		while(!Addr_Chek(ParamSave_DevAddr)) continue;
		if(ParamSave_Load(ParamSave_Ram)==0)
			break;
		else
			sta++;
	}
	return sta;
}
/**
 * @brief  CRC8 计算函数
 */
u8 CRC_Compute8(u8 *pData, u16 Len)
{
    u8 crc = 0;
    while (Len--) {
        crc ^= *pData++;
        for (u8 i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ _CRC_Value;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}
u8 Param_Handler(char* Str)
{
	char* ope_list[]={"Load","Save","Reset"};
	char ope[10]={0};
	u8 sta=FALSE, ope_idx=0;
	u8 ope_sta;
	Catch_And_Jump_Word(ope,Str,sizeof(ope)-1);
	if((ope_idx= CMD_Word_Turn_Index(ope_list,ope))==0xFF)
		goto end;

	switch (ope_idx)
	{
		case 0:
			sta= !ParamSave_Load_pack();
		break;
		case 1:
			Printf_Chx(ChSW,"Param_Save ...\r\n");
			ope_sta= ParamSave_Save(ParamSave_Ram);
			switch(ope_sta)
			{
				case 0:
					Printf_Chx(ChSW, "[Success]: Parameters saved to EEPROM.\r\n");
					break;
				case 1:
					Printf_Chx(ChSW, "[Error 1]: EEPROM Write Failed (IIC Hardware Issue).\r\n");
					break;
				case 5:
					Printf_Chx(ChSW, "[Error 5]: Write Success but Verify Failed (Interference?).\r\n");
					break;
				default:
					Printf_Chx(ChSW, "[Success]: Saved with %d retries.\r\n", ope_sta);
					break;
			}
			sta=(ope_sta!=5 && ope_sta!=1);
		break;
		case 2:
			Printf_Chx(ChSW,"Param_Reset ...\r\n");
			memset(ParamSave_Ram, 0, ParamSave_RamSize);
			sta=TRUE;
		break;
	}
	end:
	return sta;
}
