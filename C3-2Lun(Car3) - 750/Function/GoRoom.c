#include "GoRoom.h"
#include "lcd.h"
#include "stdio.h"
#include "mpu6050.h"
#include "inv_mpu.h"

static float Gyro[3];
u8 Res_num = 0;
u16 Speed_Tag = 330;

/*  
功能:开绿灯
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
功能:开红灯
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
功能:开黄灯
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
功能:转向90度		会提前5度停下防止过冲
使用方式:1左2右
*/
void Turn_Set(uint8_t Mode)
{
	uint16_t Z_Turn = 0;
	int Z_Angle = 0;
	//**转弯前加快速度
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
			//*** 右转90 度  MPU应该减小90       小于90的初始值  (越0跳360)转向目标值是LOCK+270  //LOCK+270 = 360+LOCK-90
			Motor_X(4);
			//HAL_Delay(50);
			__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,320);
			__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,320);
			if(Z_Angle <70)
			{
				Z_Turn = Z_Angle + 290;
				//重新判定
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
			//*** 左转90 度  MPU应该增加90   
			Motor_X(3);
			//HAL_Delay(50);
			__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,320);
			__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,320);
			if(Z_Angle >290)
			{
				Z_Turn = Z_Angle -290;
				//重新判定
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
锁直线巡线	循迹版  
功能:该函数将使小车尽力保持在直线上
返回值:读取到的黑/红线数量
*/
u8 Lock_Line(void )
{
	u8 Res_n = 0;
	int16_t SP_differential = 0;  //差速值储存
	Res_n = 0;
	
	if(HAL_GPIO_ReadPin(RE6_GPIO_Port,RE6_Pin)==1)	{SP_differential =150;Res_n+=1;}		//左偏限位
	if(HAL_GPIO_ReadPin(RE5_GPIO_Port,RE5_Pin)==1)	{SP_differential =100;Res_n+=1;}	//左偏
	if(HAL_GPIO_ReadPin(RE4_GPIO_Port,RE4_Pin)==1)	{SP_differential =50;Res_n+=1;}	//巡线中位
	if(HAL_GPIO_ReadPin(RE3_GPIO_Port,RE3_Pin)==1)	{SP_differential =-50;Res_n+=1;}	//巡线中位
	if(HAL_GPIO_ReadPin(RE2_GPIO_Port,RE2_Pin)==1)	{SP_differential =-100;Res_n+=1;}	//右偏	
	if(HAL_GPIO_ReadPin(RE1_GPIO_Port,RE1_Pin)==1)	{SP_differential =-150;Res_n+=1;}		//右偏限
	

	if(Res_n == 0)
	{
		//__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,300);//恢复速度
		//__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,300);//
		return Res_n;
	}//没检查到线 跳过
	
	//有线 进入差速控制段
	else if(Res_n == 1)
	{
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,320+SP_differential);//转弯降低速度转
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,320-SP_differential);//
	}
	else if(Res_n > 1)	//测到多个标记 停车
	{
		//Motor_X(0);
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,320);//恢复速度
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,320);//
	}
	return Res_n;
}


/*
题1 房间12
*/
void GOROOM1(u8 ROOM)
{	
	static uint8_t lcdbuff[20]; //LCD字符
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,350);//调试用
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,350);//调试用
	Motor_X(1);
	LCD_ShowString(4, 26, 160, 14, 14,(uint8_t * )"Whit Cross1  ");
	//**巡线等第一个十字
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
	//**确定转向方向
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
	
	//**转弯完成恢复均速
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,Speed_Tag);
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,Speed_Tag);
	printf("Turn1");
	Motor_X(1);
	LCD_ShowString(4, 26, 160, 14, 14,(uint8_t * )"Whit Cross2    ");
	
	//**巡线等停止线
	while(1)
	{
		Res_num = Lock_Line();
		if(Res_num > 2){break;}
	}
	HAL_Delay(50);
	printf("Cross2");
	Motor_X(0);
	Green_LED(1);
	//**********到达完毕
	//**********返回
	HAL_Delay(2000);
	Green_LED(0);
	Red_LED(1); 
}
