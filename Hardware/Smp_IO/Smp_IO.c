#include "./Smp_IO/Smp_IO.h"

GPIOSpeed_TypeDef DE_SPEED =  GPIO_Speed_50MHz;
char* skip(char* buff);
u8 SmpIO_StrtPin(char* Str)
{
    Str = skip(Str); 
    if (Str[0] < 'A' || Str[0] > 'G') return 0xFF; 		//错误反馈
    
    u8 num = (u8)strtol(Str + 1, NULL, 10);
    u8 pin = ((Str[0] - 'A') << 4) | (num & 0x0F);
    return pin;
}
/*还未验证
u8 SmpIO_Init(u8 Port_Pin, Mode_TypeDef Mode)
{
    GPIO_InitTypeDef STRUCT;
	static GPIO_TypeDef* const GPIO_PORT_MAP[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG};
	static const u32 GPIO_RCC_MAP[] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOC, 
                                   		RCC_APB2Periph_GPIOD, RCC_APB2Periph_GPIOE, RCC_APB2Periph_GPIOF, RCC_APB2Periph_GPIOG};
	static const u8 GPIO_Mode_MAP[] = {GPIO_Mode_IPU, GPIO_Mode_IPD, GPIO_Mode_IN_FLOATING, GPIO_Mode_AIN, 
										GPIO_Mode_Out_PP, GPIO_Mode_Out_PP, GPIO_Mode_Out_OD, GPIO_Mode_Out_OD, GPIO_Mode_AF_PP, GPIO_Mode_AF_OD,};
    u8 port_idx = Port_Pin >> 4;
    u8 pin_idx  = Port_Pin & 0x0F;

    if (port_idx> 6) return FALSE;

    // 1. 自动开启时钟
    RCC_APB2PeriphClockCmd(GPIO_RCC_MAP[port_idx], ENABLE);

    // 2. 解析模式
    Out_TypeDef Out_Data = Out_None;
	STRUCT.GPIO_Mode =GPIO_Mode_MAP[Mode];
    switch (Mode)
    {
        case PP_0: Out_Data = Out_0; break;
        case PP_1: Out_Data = Out_1; break;
        case OD_0: Out_Data = Out_0; break;
        case OD_1: Out_Data = Out_1; break;
    }

    // 3. 配置复用时钟
    if (Mode == AF_PP || Mode == AF_OD)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // 4. 执行初始化
    STRUCT.GPIO_Pin = 1 << pin_idx;
    STRUCT.GPIO_Speed = DE_SPEED;
    GPIO_Init(GPIO_PORT_MAP[port_idx], &STRUCT);

    // 5. 初始电平设定
    if (Out_Data != Out_None)
        SmpIO_Set(Port_Pin, (u8)Out_Data); 

    return TRUE;
}
*/
u8 SmpIO_Init(u8 Port_Pin, Mode_TypeDef Mode)
{
	GPIO_InitTypeDef STRUCT;
	
	GPIO_TypeDef*	PORT;
	u16 PIN;
	
	Out_TypeDef Out_Data = Out_None;
	
	u32 Init_RCC;
	
	PIN	 = 1<<(Port_Pin & 0x0F);
	switch (Port_Pin >> 4)
	{
		case A_Port:
			PORT = GPIOA;
			Init_RCC = RCC_APB2Periph_GPIOA;
			break;
		case B_Port:
			PORT = GPIOB;
			Init_RCC = RCC_APB2Periph_GPIOB;
			break;
		case C_Port:
			PORT = GPIOC;
			Init_RCC = RCC_APB2Periph_GPIOC;
			break;
		case D_Port:
			PORT = GPIOD;
			Init_RCC = RCC_APB2Periph_GPIOD;
			break;
		case E_Port:
			PORT = GPIOE;
			Init_RCC = RCC_APB2Periph_GPIOE;
			break;
		case F_Port:
			PORT = GPIOF;
			Init_RCC = RCC_APB2Periph_GPIOF;
			break;
		case G_Port:
			PORT = GPIOG;
			Init_RCC = RCC_APB2Periph_GPIOG;
			break;
		default:
			return FALSE;
	}
	switch (Mode)
	{
		case IPU:
			STRUCT.GPIO_Mode = GPIO_Mode_IPU;			break;
		
		case IPD:
			STRUCT.GPIO_Mode = GPIO_Mode_IPD;			break;

		case IF:
			STRUCT.GPIO_Mode = GPIO_Mode_IN_FLOATING;	break;

		case PP_0:
			STRUCT.GPIO_Mode = GPIO_Mode_Out_PP;
			Out_Data = Out_0;							break;

		case PP_1:
			STRUCT.GPIO_Mode = GPIO_Mode_Out_PP;
			Out_Data = Out_1;							break;

		case OD_0:
			STRUCT.GPIO_Mode = GPIO_Mode_Out_OD;	
			Out_Data = Out_0;							break;

		case OD_1:
			STRUCT.GPIO_Mode = GPIO_Mode_Out_OD;
			Out_Data = Out_1;							break;

		case AIN:
			STRUCT.GPIO_Mode = GPIO_Mode_AIN;			break;
		
		case AF_PP:
			STRUCT.GPIO_Mode = GPIO_Mode_AF_PP;			break;


		case AF_OD:
			STRUCT.GPIO_Mode = GPIO_Mode_AF_OD;			break;

		default:
			return FALSE;
	}
	STRUCT.GPIO_Pin = PIN;
	STRUCT.GPIO_Speed = DE_SPEED;
	
	RCC_APB2PeriphClockCmd(Init_RCC, ENABLE);		//使能时钟
	
	if(AF_PP<=Mode && Mode<=AF_OD)					//如果是复用模式
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//使能AFIO复用功能模块时钟 
	
	GPIO_Init(PORT, &STRUCT);						//初始化IO
	
	if(Out_Data == Out_1)
		GPIO_SetBits(PORT, PIN);
	if(Out_Data == Out_0)
		GPIO_ResetBits(PORT, PIN);
	
	return TRUE;
}
u8 inline SmpIO_Set(u8 Port_Pin, u8 Level)
{
    u8 port = Port_Pin >> 4;   // 提取 A_Port, B_Port 等
    u8 pin  = Port_Pin & 0x0F; // 提取引脚号
    
    PX_BIT(port, pin, BITOpe_W) = !!Level;
    
    return TRUE;
}
u8 inline SmpIO_Read(u8 Port_Pin)
{
    return (u8)PX_BIT(Port_Pin >> 4, Port_Pin & 0x0F, BITOpe_R);
}
void inline SmpIO_Toggle(u8 Port_Pin)
{
    u8 port = Port_Pin >> 4;
    u8 pin  = Port_Pin & 0x0F;
    
    PX_BIT(port, pin, BITOpe_W) = !PX_BIT(port, pin, BITOpe_W);
}
