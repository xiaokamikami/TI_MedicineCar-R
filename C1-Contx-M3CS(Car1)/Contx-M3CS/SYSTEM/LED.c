#include "led.h"

void LED_GPIO_Init (void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);	//打开GPIO口时钟，先打开复用才能修改复用功能
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//要先开时钟，再重映射；这句表示关闭jtag，使能swd。
	
	GPIO_InitTypeDef GPIO_InitStrue;
	GPIO_InitStrue.GPIO_Pin = R|G|B;
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStrue);
	
	GPIO_SetBits(GPIOC,R|G|B);
}

void KEY_GPIO_Init (void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);	//打开GPIO口时钟，先打开复用才能修改复用功能
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//要先开时钟，再重映射；这句表示关闭jtag，使能swd。
	
	GPIO_InitTypeDef GPIO_InitStrue;
	GPIO_InitStrue.GPIO_Pin = KEK_T|KEK_R|KEK_L;
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_IPU;
	
	GPIO_Init(GPIOC,&GPIO_InitStrue);
}

char KEY_Read (void)
{
	char N = 0;
	if(GPIO_ReadInputDataBit(GPIOC,KEK_T) == 0)
	{
		N = 1;
	}
	else if(GPIO_ReadInputDataBit(GPIOC,KEK_L) == 0)
	{
		N = 1 << 1;
	}
	else if(GPIO_ReadInputDataBit(GPIOC,KEK_R) == 0)
	{
		N = 1 << 2;
	}
	return N;
}

void LED_RGB(const char Color)
{
	GPIO_SetBits(GPIOC,R|G|B);
	switch(Color)
	{
		case 'R':	GPIO_ResetBits(GPIOC,R); break;
		case 'G':	GPIO_ResetBits(GPIOC,G); break;
		case 'B':	GPIO_ResetBits(GPIOC,B); break;
		default : break;								//不符合直接关闭所有灯
	}
}
