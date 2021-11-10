#include "tim.h"//包含tim头文件
#include "encoder.h"

int AEncoder,BEncoder;//存放从TIM4定时器读出来的编码器脉冲
int GetTim4Encoder(void)//获取TIM4定时器读出来的编码器脉冲
{
	
	BEncoder = (short)(__HAL_TIM_GET_COUNTER(&htim3));//先读取脉冲数
	__HAL_TIM_SET_COUNTER(&htim3,0);//再计数器清零
	
	return BEncoder;//返回脉冲数
	
}

int GetTim2Encoder(void)//获取TIM2定时器读出来的编码器脉冲
{
	
	AEncoder = (short)(__HAL_TIM_GET_COUNTER(&htim5));//先读取脉冲数
	__HAL_TIM_SET_COUNTER(&htim5,0);//再计数器清零
	
	return AEncoder;//返回脉冲数
	
}
