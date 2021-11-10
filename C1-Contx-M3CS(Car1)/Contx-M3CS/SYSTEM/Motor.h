#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "sys.h"

#define Motor1 1
#define Motor2 2
#define Motor3 3
#define Motor4 4

void MotorX_Init (u8 MotorX);
char *MotorX_Control(char MotorX,char Rotation,u16 Speed);
void MotorX_transfer (u8 mode);
void MotorX_Break (u8 mode);		//1×ó 2ÓÒ		µ¥ÂÖÉ²³µ
void Motor_L(u8 mode);
void Motor_R(u8 mode);
#endif
