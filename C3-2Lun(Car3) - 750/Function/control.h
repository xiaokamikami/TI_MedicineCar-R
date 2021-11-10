#ifndef __CONTROL_H
#define __CONTROL_H
#include "main.h"
//全局变量


extern int MotorOutput1;		  // 全局变量，LM电机输出
extern int MotorOutput2;		  // 全局变量，RM电机输出
//函数声明
void GetMotorPulse(void);                     // 通过TIM3读取电机脉冲并计算速度
void SetMotorVoltageAndDirection(int Pwm);    // 电机电压和方向控制函数
void Motor_X(uint8_t mode);
#endif
