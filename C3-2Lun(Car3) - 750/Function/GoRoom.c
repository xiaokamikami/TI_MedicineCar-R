#include "GoRoom.h"
#include "lcd.h"
#include "stdio.h"
u8 Res_num = 0;

/*
锁直线巡线	循迹版  
功能:该函数将使小车尽力保持在直线上
返回值:读取到的黑/红线数量
*/
u8 Lock_Line(void )
{
	u8 Res_n = 0;
	u8 SP_differential = 0;  //差速值储存
	Res_n = 0;
	
	if(HAL_GPIO_ReadPin(RE6_GPIO_Port,RE1_Pin)==1)	{SP_differential =30;Res_n+=1;}		//左偏限位
	if(HAL_GPIO_ReadPin(RE5_GPIO_Port,RE2_Pin)==1)	{SP_differential =20;Res_n+=1;}	//左偏
	if(HAL_GPIO_ReadPin(RE4_GPIO_Port,RE3_Pin)==1)	{SP_differential =10;Res_n+=1;}	//巡线中位
	if(HAL_GPIO_ReadPin(RE3_GPIO_Port,RE4_Pin)==1)	{SP_differential =0;Res_n+=1;}	//巡线中位
	if(HAL_GPIO_ReadPin(RE2_GPIO_Port,RE5_Pin)==1)	{SP_differential =-10;Res_n+=1;}	//右偏	
	if(HAL_GPIO_ReadPin(RE1_GPIO_Port,RE6_Pin)==1)	{SP_differential =-20;Res_n+=1;}		//右偏限
	

	if(Res_n == 0)
	{
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,300);//恢复速度
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,300);//
		return Res_n;
	}//没检查到线 跳过
	//有线 进入差速控制段
	else if(Res_n == 1)
	{
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,300+SP_differential);//转弯降低速度转
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,300-SP_differential);//
	}
	else if(Res_n > 1)	//测到多个标记 停车
	{
		//Motor_X(0);
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,300);//恢复速度
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,300);//
	}
	return Res_n;
}




/*
题1 房间12
*/
void GOROOM1(u8 ROOM)
{	
	Motor_X(1);
	LCD_ShowString(4, 26, 160, 14, 14,(uint8_t * )"Whit Cross1  ");
	//**巡线等第一个十字
	while(1)
	{
		Res_num = Lock_Line();
		if(Res_num >2){break;}
	}
	printf("Cross1");
	LCD_ShowString(4, 26, 160, 14, 14,(uint8_t * )"Whit Turn    ");
	
	//**确定转向方向
	if(ROOM == 1)
	{
		Turn_Set(1);
	}
	else if(ROOM == 2)
	{
		Turn_Set(2);
	}
	
	printf("Turn1");
	//**转弯完成恢复加速
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,350);
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,350);
	Motor_X(1);
	LCD_ShowString(4, 26, 160, 14, 14,(uint8_t * )"Whit Cross2    ");
	//**巡线等停止线
	while(1)
	{
		Res_num = Lock_Line();
		if(Res_num >= 2){break;}
	}
	printf("Cross2");
	Motor_X(0);
}