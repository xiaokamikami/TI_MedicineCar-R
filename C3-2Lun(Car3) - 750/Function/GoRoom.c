#include "GoRoom.h"
#include "lcd.h"
#include "stdio.h"
#include "mpu6050.h"
#include "inv_mpu.h"

static float Gyro[3];
u8 Res_num = 0;
u16 Speed_Tag = 330;

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
����:ת��90��		����ǰ5��ͣ�·�ֹ����
ʹ�÷�ʽ:1��2��
*/
void Turn_Set(uint8_t Mode)
{
	uint16_t Z_Turn = 0;
	int Z_Angle = 0;
	//**ת��ǰ�ӿ��ٶ�
	Motor_X(0);
	HAL_Delay(50);
	mpu_dmp_get_data(&Gyro[2]);  
	Z_Angle = Gyro[2];
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,335);
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,335);
	if(Mode == 2)
	{
		mpu_dmp_get_data(&Gyro[2]);  
		Z_Angle = Gyro[2];
			//*** ��ת90 ��  MPUӦ�ü�С90       С��90�ĳ�ʼֵ  (Խ0��360)ת��Ŀ��ֵ��LOCK+270  //LOCK+270 = 360+LOCK-90
			Motor_X(4);
			//HAL_Delay(50);
			__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,320);
			__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,320);
			if(Z_Angle <70)
			{
				Z_Turn = Z_Angle + 290;
				//�����ж�
				R1:
				if(Gyro[2] >180)
				{
					while(1)
					{		 
						mpu_dmp_get_data(&Gyro[2]);
						//printf("RRRF:LOCK=%.2f,Z=%.2f,TAG:%d \r\n",Z_Angle,Gyro[2],Z_Turn);
						if(Gyro[2] < Z_Turn)
						{
							Motor_X(0);
							printf("R_END");
							return;
						}
					}					
				}
				else
				{
					//printf("RRF:LOCK=%.2f,Z=%.2f,TAG:%d \r\n",Z_Angle,Gyro[2],Z_Turn);	
					mpu_dmp_get_data(&Gyro[2]);
					goto R1;
				}
			}
			else
			{
				Z_Turn = Z_Angle -70;
				while(1)
				{		 
					mpu_dmp_get_data(&Gyro[2]);
					//printf("RF:LOCK=%.2f,Z=%.2f,TAG:%d \r\n",Z_Angle,Gyro[2],Z_Turn);
					if(Gyro[2] < Z_Turn )
					{
						Motor_X(0);
						printf("R_END");
						return;
					}
				}
			}	
	}
	else if(Mode == 1)
	{
		mpu_dmp_get_data(&Gyro[2]);  
		Z_Angle = Gyro[2];
			//*** ��ת90 ��  MPUӦ������90   
			Motor_X(3);
			//HAL_Delay(50);
			__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,320);
			__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,320);
			if(Z_Angle >290)
			{
				Z_Turn = Z_Angle -290;
				//�����ж�
				L1:
				if(Gyro[2] < 180)
				{
					while(1 )
					{
						mpu_dmp_get_data(&Gyro[2]);
						//printf("LLLF:LOCK=%.2f,Z=%.2f,TAG:%d \r\n",Z_Angle,Gyro[2],Z_Turn);	
						if(Gyro[2] > Z_Turn)
						{
							Motor_X(0);
							printf("L_END");
							return;
						}		
					}					
				}
				else
				{
					//printf("LLF:LOCK=%.2f,Z=%.2f,TAG:%d \r\n",Z_Angle,Gyro[2],Z_Turn);	
					mpu_dmp_get_data(&Gyro[2]);
					goto L1;
				}
			}
			else
			{
				Z_Turn = Z_Angle +70;
				while(1)
				{
					mpu_dmp_get_data(&Gyro[2]);
					//printf("LF:LOCK=%.2f,Z=%.2f,TAG:%d \r\n",Z_Angle,Gyro[2],Z_Turn);
					if(Gyro[2] > Z_Turn )
					{
						Motor_X(0);
						printf("L_END");
						return;
					}					
				}			
			}	
	}
	Motor_X(0);

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
	static uint8_t lcdbuff[20]; //LCD�ַ�
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,350);//������
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,350);//������
	Motor_X(1);
	LCD_ShowString(4, 26, 160, 14, 14,(uint8_t * )"Whit Cross1  ");
	//**Ѳ�ߵȵ�һ��ʮ��
	while(1)
	{
		Res_num = Lock_Line();
		if(Res_num >2){break;}
	}
	HAL_Delay(50);
	Motor_X(0);
	printf("Cross1");
	LCD_ShowString(4, 26, 160, 14, 14,(uint8_t * )"Whit Turn    ");
	Yellow_LED(1);
	HAL_Delay(1000);
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
	HAL_Delay(1000);
	
	//**ת����ɻָ�����
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,Speed_Tag);
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,Speed_Tag);
	printf("Turn1");
	Motor_X(1);
	LCD_ShowString(4, 26, 160, 14, 14,(uint8_t * )"Whit Cross2    ");
	
	//**Ѳ�ߵ�ֹͣ��
	while(1)
	{
		Res_num = Lock_Line();
		if(Res_num > 2){break;}
	}
	HAL_Delay(50);
	printf("Cross2");
	Motor_X(0);
	Green_LED(1);
	//**********�������
	//**********����
	HAL_Delay(2000);
	Green_LED(0);
	Red_LED(1); 
}
