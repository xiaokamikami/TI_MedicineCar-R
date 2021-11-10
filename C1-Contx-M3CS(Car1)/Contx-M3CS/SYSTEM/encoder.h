#ifndef _ENCODER_H
#define _ENCODER_H


#include "stm32f10x.h"
#include "encoder.h"
#include "main.h"
#define ENCODER_TIM_PERIOD 65534//65535
extern int leftWheelEncoderNow    ;
extern int rightWheelEncoderNow   ;
void Encoder_Init_TIM3(void);		//左电机
void Encoder_Init_TIM5(void);		//右电机
s16 getTIMx_DetaCnt(TIM_TypeDef * TIMx);
int Get_Motor_Speed(int *leftSpeed,int *rightSpeed); //计算速度


#endif
