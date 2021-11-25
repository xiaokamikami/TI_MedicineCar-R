#include "infrared.h"
#include "usart.h" 
#include "main.h"

int out1,out2,out3,out4,out5,out6,angle; //四路红外全局变量，舵机角度输出；
unsigned int state,Idata;     //巡线状态，红外函数输出数据

unsigned int InfraredScan()
{
	
	unsigned int state=0;
	
	out1 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14);
	out2 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15);
	out3 = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8);
	out4 = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9);
    out5 = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_10);
    out6 = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_11);//取出六路红外的数据
	
	if(out1 == 1){state = 1;}//IN1，4级右转向
	else if(out2 == 1){state = 2;}//IN2，2级右转向
	if(out3 == 1){state = 4;}//IN3，直线循迹，右修
	else if(out4 == 1){state = 8;}//IN4，直线循迹，左修
    if(out5 == 1){state = 16;}//IN5，2级左转向
    else if(out6 == 1){state = 32;}//IN6，4级左转向

//	if(out3==1&&out2==1){state = 6;}//1级右转向
//	if(out5==1&&out4==1){state = 24;}//1级左小转向
//
//    if(out2==1&&out1==1){state = 3;}//3级右转向
//    if(out6==1&&out5==1){state = 48;}//3级左转向
    Idata = out1+out2+out3+out4+out5+out6;
	if(out1==1&&out2==1&&out3==1&&out4==1){state = 0x0f;}//小车抱起
    else if(out1==0&&out2==0&&out3==0&&out4==0){state = 0;}//空白区域
	else if(Idata > 2){state = 0x0d;}
	return state;
	
}	
