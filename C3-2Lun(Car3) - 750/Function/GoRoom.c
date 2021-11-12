#include "GoRoom.h"
#include "lcd.h"
#include "stdio.h"
u8 Res_num = 0;

/*
��ֱ��Ѳ��	ѭ����  
����:�ú�����ʹС������������ֱ����
����ֵ:��ȡ���ĺ�/��������
*/
u8 Lock_Line(void )
{
	u8 Res_n = 0;
	u8 SP_differential = 0;  //����ֵ����
	Res_n = 0;
	
	if(HAL_GPIO_ReadPin(RE6_GPIO_Port,RE1_Pin)==1)	{SP_differential =30;Res_n+=1;}		//��ƫ��λ
	if(HAL_GPIO_ReadPin(RE5_GPIO_Port,RE2_Pin)==1)	{SP_differential =20;Res_n+=1;}	//��ƫ
	if(HAL_GPIO_ReadPin(RE4_GPIO_Port,RE3_Pin)==1)	{SP_differential =10;Res_n+=1;}	//Ѳ����λ
	if(HAL_GPIO_ReadPin(RE3_GPIO_Port,RE4_Pin)==1)	{SP_differential =0;Res_n+=1;}	//Ѳ����λ
	if(HAL_GPIO_ReadPin(RE2_GPIO_Port,RE5_Pin)==1)	{SP_differential =-10;Res_n+=1;}	//��ƫ	
	if(HAL_GPIO_ReadPin(RE1_GPIO_Port,RE6_Pin)==1)	{SP_differential =-20;Res_n+=1;}		//��ƫ��
	

	if(Res_n == 0)
	{
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,300);//�ָ��ٶ�
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,300);//
		return Res_n;
	}//û��鵽�� ����
	//���� ������ٿ��ƶ�
	else if(Res_n == 1)
	{
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,300+SP_differential);//ת�併���ٶ�ת
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,300-SP_differential);//
	}
	else if(Res_n > 1)	//�⵽������ ͣ��
	{
		//Motor_X(0);
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,300);//�ָ��ٶ�
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,300);//
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
	
	//**ȷ��ת����
	if(ROOM == 1)
	{
		Turn_Set(1);
	}
	else if(ROOM == 2)
	{
		Turn_Set(2);
	}
	
	printf("Turn1");
	//**ת����ɻָ�����
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,350);
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,350);
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
}