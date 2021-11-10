#include "SPI.h"
#include "delay.h"

/*

*/

//#include "stc8.h"

//sbit SCLK_1 = P1 ^ 0; // SYSTIME
//sbit MISO_1 = P1 ^ 1; //(SDI)	master input slave output
//sbit MOSI_1 = P1 ^ 2; //(SDO)	master input
//sbit CS_1  = P1 ^ 3;	  // EN

//sbit SCLK_2 = P4 ^ 0; // SYSTIME
//sbit MISO_2 = P4 ^ 2; //(SDI)	master input slave output
//sbit MOSI_2 = P4 ^ 1; //(SDO)	master input
//sbit CS_2  = P4 ^ 4;	  // EN

 #include "sys.h"

 #define SCLK_1 PBout(13)										// SYSTIME
 #define MISO_1 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)		//(SDI)	master input slave output
 #define MOSI_1 PBout(15)										//(SDO)	master output slave input
 #define CS_1   PBout(12)										// EN


 #define SCLK_2 PAout(0)										// SYSTIME
 #define MISO_2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)		//(SDI)	master input slave output
 #define MOSI_2 PAout(2)										//(SDO)	master output slave input
 #define CS_2   PAout(3)										// EN

char SPI_Flag = 0;
char SPI_array_s[20];
unsigned int Time = 10;

//void delay_us(unsigned char x) //@24.000MHz
//{
//	Delayx0us(x);
//}

void GPIO_u(char num)
{
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);				//禁用 jtag
	 if(num == 1 || num == '1')
	 {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		GPIO_InitTypeDef GPIO_InitStrue;
		GPIO_InitStrue.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_15;
		GPIO_InitStrue.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB,&GPIO_InitStrue);
		
		GPIO_InitStrue.GPIO_Pin = GPIO_Pin_14;
		GPIO_InitStrue.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOB,&GPIO_InitStrue);
		
		GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15);
	 }

	 else if(num == 2 || num == '2')
	 {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);			// B1
		GPIO_InitTypeDef GPIO_InitStrue;
		GPIO_InitStrue.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3;	// B2
		GPIO_InitStrue.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStrue);								// B3
		
		GPIO_InitStrue.GPIO_Pin = GPIO_Pin_1;							// B4
		GPIO_InitStrue.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA,&GPIO_InitStrue);								// B5
		
		GPIO_SetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_2| GPIO_Pin_3);		// B6
	 }
	num = 0;
}

char SPI_Write_And_Read_Byte(char num, char dat) //Master
{
	char ret_data = 0;
	char n, m = 0x80;

	if (num == 1 || num == '1')
	{
		MOSI_1 = 1;
		SCLK_1 = 1;
		CS_1 = 0;
		delay_us(Time);
		for (n = 0; n < 8; n++)
		{
			SCLK_1 = 0;

			if (((dat << n) & m) == m)
			{
				MOSI_1 = 1;
			}
			else
			{
				MOSI_1 = 0;
			}

			delay_us(Time);

			SCLK_1 = 1;
			if (MISO_1 == 1)
				ret_data |= (m >> n);
			else
				ret_data |= (0 >> n);
			delay_us(Time);
		}
		MOSI_1 = 1;
		SCLK_1 = 1;
		CS_1 = 1;
	}
	else if (num == 2 || num == '2')
	{
		MOSI_2 = 1;
		SCLK_2 = 1;
		CS_2 = 0;
		delay_us(Time);
		for (n = 0; n < 8; n++)
		{
			if (((dat << n) & m) == m)
			{
				MOSI_2 = 1;
			}
			else
			{
				MOSI_2 = 0;
			}
			SCLK_2 = 0;
			delay_us(Time);

			SCLK_2 = 1;
			if (MISO_2 == 1)
				ret_data |= (m >> n);
			else
				ret_data |= (0 >> n);
			delay_us(Time);
		}
		MOSI_2 = 1;
		SCLK_2 = 1;
		CS_2 = 1;
	}

	return ret_data;
}

char *SPI_Write_And_Read(char num, const char *array, char Len)
{
	char n;

	for (n = 0; n < 20; n++)
	{
		SPI_array_s[n] = 0;
	}

	for (n = 0; n < Len; n++)
	{
		SPI_array_s[n] = SPI_Write_And_Read_Byte(num, *(array + n));
		delay_us(Time);
	}
	return SPI_array_s;
}

void SPI_Software_Init(char num)
{
	GPIO_u(num);
}

void SPI_SetSpeed(unsigned int SPI_BaudRatePrescaler_x) //
{
	Time = (SPI_BaudRatePrescaler_x + 1) * 2;
}
