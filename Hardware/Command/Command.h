#ifndef _COMMAND_H
#define _COMMAND_H

#include "./Base_Head_File.h"

typedef u8 (*CMD_CB)(char*);

typedef struct
{
	char* cmd;
	CMD_CB CallBack;
	char* info;
	char* CMD_INFO;
}Command;

u8 Help_Handle(char* str);
u8 Print_ModeInfo(char* Str);
void Command_Call(void);
void Handle_Rec_Command(char* str, u16 len);
char* skip(char* buff);										//Ìø¹ý¶à¿Õ¸ñ
u8 Catch_Word(char* Dest, char** Src, u8 Max_num);
char* Catch_And_Jump_Word(char* Dest, char* Src,  u16 Max_num);
u8 CMD_Catch_Some(char** Buff, char* Str, ...);
u8 CMD_Word_Turn_Index(char** Buff, char* Cmd);
u8 CMD_Word_T_Some(u8* Indexs, char** CMDs, ...);

#endif

