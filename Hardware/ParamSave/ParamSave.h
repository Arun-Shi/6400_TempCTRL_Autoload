#ifndef __PARAMSAVE_H__
#define __PARAMSAVE_H__

typedef struct {
	u8* Data;
    u16 Sys_Version;
    u16 Magic_Num;
    u8  Crc_Num;
}ParamSave_T;

u8 CRC_Compute8(u8 *pData, u16 Len);
u8 ParamSave_Save(void* DataBuff);
u8 ParamSave_Load(void* DataBuff);
u8 ParamSave_Load_pack(void);
u8 Param_Handler(char* Str);

#endif
