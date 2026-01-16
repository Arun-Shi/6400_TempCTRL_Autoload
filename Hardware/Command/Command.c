#include "./Command/Command.h"

#define HELP_CMD_Info	"使用 ?+指令模块 可查询指令详细参数\r\n"

#define Param_CMDINFO		"\r\n参数指令\r\n\
\t-Save   保存系统参数，将当前参数固化\r\n\
\t-Load   加载系统参数，即恢复上次保存的参数，将覆盖未保存参数\r\n\
\t-Reset  恢复系统默认参数\r\n\
\t例：Param Save ->保存系统参数。\r\n"

#define Sys_CMDINFO			"\r\n设置指令\
\t-TempCTRL     控制温度控制功能状态\r\n\
\t-Set_TarTemp  设置温度目标值\r\n\
\t-Get_Data     获取当前温度数据\r\n\
\t-Set_Button   按键控制功能的使能切换\r\n\
\t例：Sys Set_TarTemp 50 ->设置温度目标值为50℃\r\n\
\t例：Sys TempCTRL Enable ->开启温度控制\r\n"

Command cmd_list[] = //"命令词",回调函数,"命令信息"
{
	{"Help",	Help_Handle,		"帮助指令",						HELP_CMD_Info},
	{"?",		Print_ModeInfo,		"查询,? xx查询具体指令",		HELP_CMD_Info},
	{"Param",	Param_Handler,		"参数指令",						Param_CMDINFO},
	{"Sys",		Sys_Handler,		"设置指令",						Sys_CMDINFO},
	NULL,
};
u8 Sys_Handler(char* Buff)
{
//定义变量
	u8 STA=TRUE;
	char str_temp[20]={0};
//抓取词条
	Buff= Catch_And_Jump_Word(str_temp, Buff, 19);
//比对词条并执行函数取返回值
	if(strncasecmp(str_temp,"TempCTRL",8)==0)			//控制温控功能状态(当前直接控制3路)
	{
		Catch_And_Jump_Word(str_temp, Buff, 7);
		if(strncasecmp(str_temp,"Disable",7)==0)
			{
				PID_Switch(&PID_TempLeft ,	FALSE);
				PID_Switch(&PID_TempMid	 ,	FALSE);
				PID_Switch(&PID_TempRight,	FALSE);
			}
		else if(strncasecmp(str_temp,"Enable",6)==0)
			{
				PID_Switch(&PID_TempLeft,	TRUE);
				PID_Switch(&PID_TempMid	,	TRUE);
				PID_Switch(&PID_TempRight,	TRUE);
			}
		else
			STA=FALSE;
	}
	else if(strncasecmp(str_temp,"Set_TarTemp",11)==0)	//设置期望温度(当前直接控制3路)
		{
			Catch_And_Jump_Word(str_temp, Buff, 8);
			float target= strtof(str_temp, NULL);
			if(__Range_Judg(target,20, __Value_MAXTemp-5))	//判断温度是否在合理范围内
			{
				PID_Set_Target(&PID_TempLeft ,target);
				PID_Set_Target(&PID_TempMid  ,target);
				PID_Set_Target(&PID_TempRight,target);
			}
			else
				STA=FALSE;
		}
	else if(strncasecmp(str_temp,"Get_Data",7)==0)		//获取信息
	{
		Printf_Chx(ChSW,"Cur_temp Left:%.2f℃\r\n",Temperature_OFBox[0]);
		Printf_Chx(ChSW,"Cur_temp Mid:%.2f℃\r\n",Temperature_OFBox[1]);
		Printf_Chx(ChSW,"Cur_temp Right:%.2f℃\r\n",Temperature_OFBox[2]);

		Printf_Chx(ChSW,"Target:%.2f℃\r\n",PID_TempLeft.Value.Target/125);
		
		Printf_Chx(ChSW,"Temp_Ctrl status:%s\r\n", PID_TempLeft.PID_Switch?"Enable":"Disable");
	}
	else if(strncasecmp(str_temp,"Set_Button",10)==0)	//控制按键轮询功能的使能状态
	{
		Catch_And_Jump_Word(str_temp, Buff, 7);
		if(strncasecmp(str_temp,"Disable",7)==0)
			Stepper_RunEN=FALSE;
		else if(strncasecmp(str_temp,"Enable",6)==0)
			Stepper_RunEN=TRUE;
		else
			STA=FALSE;
	}
	else
		STA=FALSE;
//没有匹配返回FALSE
	return STA;
}
/*
描述：指令扫描，获取接收数据包中的指令
参数：数据包地址
返回：1- 执行失败	0- 执行成功
备注：获取指令成功后直接执行，无指令则输出错误
*/
u8 cmd_scan(char* buff)				//命令扫描
{
	char str_temp[32]={0};
	u8 size = sizeof(cmd_list)/sizeof(Command);							//获取指令库尺寸

	Catch_And_Jump_Word(str_temp,buff,31);
	for(u8 i= 0; i< size; i++)											//将接收到的包与指令库对比
	{
		u8 len= __Bigger(strlen(cmd_list[i].cmd), strlen(str_temp));	//长度定为两者中较大的
		if(strncasecmp( buff, cmd_list[i].cmd, len) == 0)				//对比数据包和指令，相等为0
		{
			buff+=len;
			buff=skip(buff);
			
			u8 back=cmd_list[i].CallBack(buff);							//执行命令
			
			if(back)
				Printf_Chx(ChSW,"Pass\r\n");
			else
				Printf_Chx(ChSW,"Fail\r\n");

			return TRUE;
		}
	}
	Printf_Chx(ChSW,"FAIL: %s :Unkown command\r\n",(char*)UART1_RX.RXPacket);
	return FALSE;
}
void Command_Call(void)
{
//缓存
	char* buff= pvPortMalloc(UART1_RX.len+ 1);
	memmove(buff, UART1_RX.RXPacket, UART1_RX.len);
	UART1_RX.len=0;
//执行，销毁
	cmd_scan(buff);
	vPortFree(buff);
}

u8 Print_ModeInfo(char* Str)
{
	char str_temp[32]={0};
	for(u8 i= 1; cmd_list[i].cmd!=NULL; i++){							//将接收到的包与指令库对比
		Catch_And_Jump_Word(str_temp,Str,31);
		u8 len= __Bigger(strlen(cmd_list[i].cmd), strlen(str_temp));	//长度定为两者中较大的
		
		if(strncasecmp( Str, cmd_list[i].cmd, len) == 0)				//对比数据包和指令，相等为0
			if(cmd_list[i].CMD_INFO)
			{
				size_t offset=0;
				while(1)
				{
					Printf_Chx(ChSW,"%.511s",cmd_list[i].CMD_INFO+ offset);
					
					if(strlen(cmd_list[i].CMD_INFO+ offset)>511)
						offset+=511;
					else
						break;

				}
				return TRUE;
			}
	}
	Printf_Chx(ChSW,"FAIL: %s :Unkown command\r\n",(char*)UART1_RX.RXPacket);
	return FALSE;
}
u8 Help_Handle(char* Str)
{
//	if(*(Str-2)=='?'){
//		char str_temp[32]={0};
//		for(u8 i= 1; cmd_list[i].cmd!=NULL; i++){							//将接收到的包与指令库对比
//			Catch_And_Jump_Word(str_temp,Str,31);
//			u8 len= __Bigger(strlen(cmd_list[i].cmd), strlen(str_temp));	//长度定为两者中较大的
//			
//			if(strncasecmp( Str, cmd_list[i].cmd, len) == 0)				//对比数据包和指令，相等为0
//				if(cmd_list[i].CMD_INFO)
//					Printf_Chx(ChSW,"%s",cmd_list[i].CMD_INFO);
//		}
//	}
//	else
//	{
		for(u8 i=0; cmd_list[i].cmd!=NULL;i++)
			if(cmd_list[i].cmd&& cmd_list[i].info)
				Printf_Chx(ChSW, "%s		%s\r\n", cmd_list[i].cmd, cmd_list[i].info);
//	}
	return 1;
}
char* skip(char* buff)				//跳过多空格
{
	while(*buff == ' ')
	{
		buff++;
	}
	return buff;
}
/*
说	明：抓取一条指令，并返回指令的末尾后一位的空格
参	数：Dest- 指向存储截取出来指令的缓存
		Src-  指向源字符串
		Max_num- 最大截取字节数
返	回：指向下一条指令起始位置的指针
备	注：指令间用空格的ASCII码（0x20）分割，结尾用‘\0’标记
*/
char* Catch_And_Jump_Word(char* Dest, char* Src,  u16 Max_num)
{
	if(Dest==NULL|| Src==NULL || Max_num==0)
		return NULL;

	u8 i;
//跳过前导空格
	while (*Src == ' ') Src++;

//抓取当前指令
	for (i = 0; i< Max_num && Src[i] != ' ' && Src[i] !=0; i++)
		Dest[i] = Src[i];
	Dest[i]=0;				//结尾加'\0'
	if(Src[i]==0)
		return NULL;
//返回指令的末尾后一位的空格
	return Src+i+1;
}
/*
函	数：抓取一些关键词，合法的关键词格式为:xx yy，此时xx为前缀，yy为值
参	数：Buff-	缓存数组指针
		Str-	传入的待解析字符串
		可变参数-	传入复数的字符串
返	回：总的匹配抓取数
实	现：使用strstr匹配可变参数的每一字符串，抓取其后方的字符串依次放入Buff中
备	注：1、可变字符串的最后，需要用NULL作为结束
		2、前缀需要区分大小写
		3、关键词最大支持256字节
		4、没有越位检测
*/
u8 CMD_Catch_Some(char** Buff, char* Str, ...)
{
	va_list args;
	u8 count = 0;
	char* current_arg;

//初始化可变参数列表
	va_start(args, Str);

//遍历所有可变参数，直到遇到NULL
	while ((current_arg = va_arg(args, char*)) != NULL)
	{
	//使用strstr查找当前参数是否在Str中
		char* found_pos = strstr(Str, current_arg);
		if (found_pos != NULL)
		{
	//计算关键词后面的字符串位置
		char* result = found_pos + strlen(current_arg);
	//将结果存入Buff的下一个位置
		Catch_And_Jump_Word(Buff[count], result, 256);
		count++;
		}
	}
//结束可变参数遍历
	va_end(args);

	return count;
}
/*
函	数：找出Cmd在Buff中的索引
参	数：Buff-	库
		Cmd-	需要匹配的关键词
返	回：0xFF-	在库中没找到Cmd
实	现：遍历库，与Cmd对比
备	注：Buff末尾需要有NULL作结尾约束
		Cmd不分大小写
*/
u8 CMD_Word_Turn_Index(char** Buff, char* Cmd)
{
	for(u8 i=0;Buff[i]!=NULL;i++)
	{
		u8 len=__Min_OF(strlen(Buff[i]),strlen(Cmd));
		if(strncasecmp(Buff[i], Cmd, len)==0)
			return i;
	}
	return 0xFF;
}
/*
函	数：找出CMDs在Buffs中的索引
参	数：Indexs-	索引输出
		Cmds-	需要匹配的关键词串
返	回：TRUE-	全部匹配成功1
		FALSE-	不全部匹配成功
实	现：循环调用CMD_Word_Turn_Index
备	注：Buffs末尾需要有NULL作结尾约束
*/
u8 CMD_Word_T_Some(u8* Indexs, char** CMDs, ...)
{
	va_list args;
	u8 status=TRUE;
	u8 count=0;
	char** current_arg;

//初始化可变参数列表
	va_start(args, CMDs);
	//遍历所有可变参数，直到遇到NULL
	while((current_arg = va_arg(args, char**)) != NULL)
	{
		u8 found_index= CMD_Word_Turn_Index(current_arg, CMDs[count]);
		if (found_index != 0xFF)
		{
			Indexs[count]=found_index;
			count++;
		}
		else
			status=FALSE;
	}
//结束可变参数遍历
	va_end(args);

	return status;
}
