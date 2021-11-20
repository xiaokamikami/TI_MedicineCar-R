#include "GoRoom.h"
#include "lcd.h"
#include "stdio.h"
u8 Res_num = 0;


/*  
����:���̵�
*/
void Green_LED(u8 i)
{
	if(i == 1)
	{
		HAL_GPIO_WritePin(GREEN_GPIO_Port,GREEN_Pin,GPIO_PIN_RESET);
	}
	else if(i == 0)
	{
		HAL_GPIO_WritePin(GREEN_GPIO_Port,GREEN_Pin,GPIO_PIN_SET);
	}
}
/*  
����:�����
*/
void Red_LED(u8 i)
{
	
	if(i == 1)
	{
		HAL_GPIO_WritePin(RED_GPIO_Port,RED_Pin,GPIO_PIN_RESET);
	}
	else if(i == 0)
	{
		HAL_GPIO_WritePin(RED_GPIO_Port,RED_Pin,GPIO_PIN_SET);
	}
}

/*  
����:���Ƶ�
*/
void Yellow_LED(u8 i)
{
	
	if(i == 1)
	{
		HAL_GPIO_WritePin(RED_GPIO_Port,RED_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GREEN_GPIO_Port,GREEN_Pin,GPIO_PIN_RESET);
	}
	else if(i == 0)
	{
		HAL_GPIO_WritePin(RED_GPIO_Port,RED_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GREEN_GPIO_Port,GREEN_Pin,GPIO_PIN_SET);
	}
}
		  

/*
��ֱ��Ѳ��	ѭ����  
����:�ú�����ʹС������������ֱ����
����ֵ:��ȡ���ĺ�/��������
*/
u8 Lock_Line(void )
{
	u8 Res_n = 0;
	int16_t SP_differential = 0;  //����ֵ����
	Res_n = 0;
	
	if(HAL_GPIO_ReadPin(RE6_GPIO_Port,RE6_Pin)==1)	{SP_differential =150;Res_n+=1;}		//��ƫ��λ
	if(HAL_GPIO_ReadPin(RE5_GPIO_Port,RE5_Pin)==1)	{SP_differential =100;Res_n+=1;}	//��ƫ
	if(HAL_GPIO_ReadPin(RE4_GPIO_Port,RE4_Pin)==1)	{SP_differential =50;Res_n+=1;}	//Ѳ����λ
	if(HAL_GPIO_ReadPin(RE3_GPIO_Port,RE3_Pin)==1)	{SP_differential =-50;Res_n+=1;}	//Ѳ����λ
	if(HAL_GPIO_ReadPin(RE2_GPIO_Port,RE2_Pin)==1)	{SP_differential =-100;Res_n+=1;}	//��ƫ	
	if(HAL_GPIO_ReadPin(RE1_GPIO_Port,RE1_Pin)==1)	{SP_differential =-150;Res_n+=1;}		//��ƫ��
	

	if(Res_n == 0)
	{
		//__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,300);//�ָ��ٶ�
		//__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,300);//
		return Res_n;
	}//û��鵽�� ����
	
	//���� ������ٿ��ƶ�
	else if(Res_n == 1)
	{
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,320+SP_differential);//ת�併���ٶ�ת
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,320-SP_differential);//
	}
	else if(Res_n > 1)	//�⵽������ ͣ��
	{
		//Motor_X(0);
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,320);//�ָ��ٶ�
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,320);//
	}
	return Res_n;
}


/*
��1 ����12
*/
void GOROOM1(u8 ROOM)
{	
	Motor_X(1);
	LCD_ShowString(4, 26, 160, 14, 14,(uint8_t * )"Whit Cross1  ");
	
	//**Ѳ�ߵȵ�һ��ʮ��
	while(1)
	{
		Res_num = Lock_Line();
		if(Res_num >2){break;}
	}
	printf("Cross1");
	LCD_ShowString(4, 26, 160, 14, 14,(uint8_t * )"Whit Turn    ");
	Yellow_LED(1);

	//**ȷ��ת����
	if(ROOM == 1)
	{
		Turn_Set(1);
	}
	else if(ROOM == 2)
	{
		Turn_Set(2);
	}
	Yellow_LED(0);
	printf("Turn1");

	Motor_X(1);
	LCD_ShowString(4, 26, 160, 14, 14,(uint8_t * )"Whit Cross2    ");
	//**Ѳ�ߵ�ֹͣ��
	while(1)
	{
		Res_num = Lock_Line();
		if(Res_num >= 2){break;}
	}
	printf("Cross2");
	Motor_X(0);
	
	Green_LED(1);
	//**********�������
	//**********����
	HAL_Delay(2000);
	Green_LED(0);
	Yellow_LED(1); 
}
