#include "HC595.h"

void HC595_Init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// GPIOA 时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,RCLK | SRCLK | SER_Data);
}

void Send_HC595 (const char Dat)
{
	char n;
	GPIO_ResetBits(GPIOB,RCLK);
	GPIO_ResetBits(GPIOB,SRCLK);
    for(n = 0;n < 8;n++)								//上升沿有效
    {
        if((Dat << n) & 0x80) GPIO_SetBits(GPIOB,SER_Data);
        else       			  GPIO_ResetBits(GPIOB,SER_Data);
        GPIO_ResetBits(GPIOB,SRCLK);
		GPIO_SetBits(GPIOB,SRCLK);
    }
}

void HC595 (const char *Dat,char num)
{
	char m;
	GPIO_ResetBits(GPIOB,RCLK);
	GPIO_ResetBits(GPIOB,SRCLK);
	for(m = 0; m < num;m++)
	{
		Send_HC595 (*(Dat+m));
	}
	GPIO_ResetBits(GPIOB,RCLK);
	GPIO_SetBits(GPIOB,RCLK);
}

void HC595_Byte (const char Dat)
{
	Send_HC595 (Dat);
	GPIO_ResetBits(GPIOB,RCLK);
	GPIO_SetBits(GPIOB,RCLK);
}
