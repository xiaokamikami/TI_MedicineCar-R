#include "Button.h"
#include "main.h"
#include "tim.h"
#include "control.h"
#include "stm32f1xx_it.h"

int Count;//i代表int型变量， ButtonCount表示按键计数变量
int Flag;//i代表int型变量， ButtonFlag表示重按键标志， 1代表重新按键， 0为没有重新按键
int ButtonState;//g是globle代表全局变量， 会在其他地方引用； i代表int型变量， ButtonState表示按键标志， 1代表按下， 0代表松开

void ButtonScan(void)
{
	if( HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == GPIO_PIN_SET )//如果引脚检测到高电平
	{
		Count++; //按键按下， 计数iButtonCount加1
		if(Count >= 30) //1ms中断服务函数里运行一次， iButtonCount大于等于30， 即按键已稳定按下30ms
		{
			
			if(Flag == 0) //判断有没有重按键， 1为有， 0为没有
			{
				
				if(ButtonState == 0)  ButtonState = 1; //设置按键标志
				else                  ButtonState = 0;//按下后不为 1 就为 0，1启动；0停止
				
				Count = 0;
				Flag = 1; //设置重按键标志
				
			}
			else Count = 0;//如果重按键， 则重新计数
			
		}
//		else ButtonState=0;//如果没有稳定按下30ms， 则代表没有按下按键
		
	}
	else //如果一直无检测到高电平， 即一直无按键按下
	{
		Count = 0; //清零iButtonCount
//		ButtonState=0; //清除按键标志
		Flag = 0; //清除重按键标志
	}
	
	if(ButtonState == 0)
	{
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1,0); 
		__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);//A电机
	
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3,0); 
		__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_4,0);//B电机
	}
	
}
