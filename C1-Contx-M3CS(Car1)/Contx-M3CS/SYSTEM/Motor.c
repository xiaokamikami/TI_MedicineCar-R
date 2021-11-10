#include "motor.h"
#include "PWM.h"
#include "DATA_handle.h"

char Num_Motor[4][16]; //"Motx:R/L (Speed) %"

void MotorX_transfer (u8 mode)		//1正2反
{
	if(mode == 1)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_2 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_3 );
		GPIO_SetBits(GPIOC, GPIO_Pin_0 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_1 );
	}
	else if (mode == 2)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_3 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_2 );
		GPIO_SetBits(GPIOC, GPIO_Pin_1 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_0 ); 
	}
	else if(mode == 0)
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_2 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_3 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_0 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_1 ); 
	}
}
void Motor_L(u8 mode)
{
	if(mode == 1)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_2 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_3 );

	}
	else if (mode == 2)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_3 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_2 );

	}
}
void Motor_R(u8 mode)
{
	if(mode == 1)
	{

		GPIO_SetBits(GPIOC, GPIO_Pin_0 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_1 );
	}
	else if (mode == 2)
	{

		GPIO_SetBits(GPIOC, GPIO_Pin_1 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_0 ); 
	}
}
void MotorX_Break (u8 mode)		//1左 2右单轮刹车   3左 4右 单轮反转
{
	if(mode == 1)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_1 );
		GPIO_SetBits(GPIOC, GPIO_Pin_0 ); 
	}
	else if (mode == 2)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_2 );
		GPIO_SetBits(GPIOC, GPIO_Pin_3 );

	}
	else if (mode == 3)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_3 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_2 );
		GPIO_SetBits(GPIOC, GPIO_Pin_0 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_1 );
	}
	else if (mode == 4)
	{

		GPIO_SetBits(GPIOC, GPIO_Pin_2 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_3 );
		GPIO_SetBits(GPIOC, GPIO_Pin_1 );
		GPIO_ResetBits(GPIOC, GPIO_Pin_0 );
	}

}
void MotorX_Init(u8 MotorX)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;

	//TIM8_PWM_Init(1000, 719); //1000 * 720 / 72MHz = 10MS
	switch (MotorX)
	{
		case 1:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //GPIO C

			GPIO_InitStrue.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
			GPIO_Init(GPIOC, &GPIO_InitStrue);

//			GPIO_SetBits(GPIOC, GPIO_Pin_2 );
//			GPIO_ResetBits(GPIOC,GPIO_Pin_3);
			break;
		}
		case 2:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //GPIO C

			GPIO_InitStrue.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
			GPIO_Init(GPIOC, &GPIO_InitStrue);

//			GPIO_SetBits(GPIOC, GPIO_Pin_0 );
//			GPIO_ResetBits(GPIOC, GPIO_Pin_1);
			break;
		}
//	case 3:
//	{
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //GPIO B
//		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
//		GPIO_InitStrue.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
//		GPIO_Init(GPIOB, &GPIO_InitStrue);

//		GPIO_SetBits(GPIOB, GPIO_Pin_4 | GPIO_Pin_5);
//		break;
//	}
//	case 4:
//	{
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //GPIO C
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); //GPIO D

//		GPIO_InitStrue.GPIO_Pin = GPIO_Pin_12;
//		GPIO_Init(GPIOC, &GPIO_InitStrue);
//		GPIO_InitStrue.GPIO_Pin = GPIO_Pin_2;
//		GPIO_Init(GPIOD, &GPIO_InitStrue);

//		GPIO_SetBits(GPIOC, GPIO_Pin_12);
//		GPIO_SetBits(GPIOD, GPIO_Pin_2);
//		break;
//	}
	default:
		break;
	}
}

void SpeedNum_Handle(char MotorX,char Rotation,u16 Speed)
{
	char num_code[16] = "Motx:P 00 %  \r\n";
	num_code[3] = MotorX + '0';
	num_code[4] = Rotation;
	if (Speed >= 1000)
	{
		num_code[7] = 'M';
		num_code[8] = 'A';
		num_code[9] = 'X';
	}
	else
	{
		num_code[7] = (Speed / 100) % 10 + '0';
		num_code[8] = (Speed / 10) % 10 + '0';
		num_code[9] = ' ';
	}
}

char *MotorX_Control(char MotorX, char Rotation, u16 Speed)
{
	switch (MotorX)
	{
	case 1:
	{
		if (Rotation == 'r' || Rotation == 'R') //Right 右
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
			GPIO_SetBits(GPIOC, GPIO_Pin_3);
		}
		else if (Rotation == 'l' || Rotation == 'L') //Left	左
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_2);
			GPIO_ResetBits(GPIOC, GPIO_Pin_3);
		}
		else
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_2 | GPIO_Pin_3);
		} //Stop

		TIM_SetCompare1(TIM8, Speed);
		break;
	}
	case 2:
	{
		if (Rotation == 'r' || Rotation == 'R')
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_0);
			GPIO_SetBits(GPIOC, GPIO_Pin_1);
		}
		else if (Rotation == 'l' || Rotation == 'L')
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_0);
			GPIO_ResetBits(GPIOC, GPIO_Pin_1);
		}
		else
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1);
		} //Stop

		TIM_SetCompare2(TIM8, Speed);

		break;
	}
	case 3:
	{
		if (Rotation == 'r' || Rotation == 'R')
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_4);
			GPIO_SetBits(GPIOB, GPIO_Pin_5);
		}
		else if (Rotation == 'l' || Rotation == 'L')
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_4);
			GPIO_ResetBits(GPIOB, GPIO_Pin_5);
		}
		else
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_4 | GPIO_Pin_5);
		} //Stop

		TIM_SetCompare3(TIM8, Speed);
		break;
	}
	case 4:
	{
		if (Rotation == 'r' || Rotation == 'R')
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_12);
			GPIO_SetBits(GPIOD, GPIO_Pin_2);
		}
		else if (Rotation == 'l' || Rotation == 'L')
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_12);
			GPIO_ResetBits(GPIOD, GPIO_Pin_2);
		}
		else
		{
			GPIO_ResetBits(GPIOD, GPIO_Pin_2);
			GPIO_ResetBits(GPIOC, GPIO_Pin_12);
		} //Stop

		TIM_SetCompare4(TIM8, Speed);
		
		break;
	}
	default:
		break;
	}
	SpeedNum_Handle(MotorX,Rotation,Speed);

}
