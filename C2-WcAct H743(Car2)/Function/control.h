#ifndef __CONTROL_H
#define __CONTROL_H


//全局变量
extern int MotorSpeed1;  // 全局变量，LM电机当前速度数值，从编码器中获取
extern int MotorSpeed2;  // 全局变量，RM电机当前速度数值，从编码器中获取
extern int SpeedTarget1;	  // 全局变量，LM速度目标值
extern int SpeedTarget2;	  // 全局变量，RM速度目标值
extern int MotorOutput1;		  // 全局变量，LM电机输出
extern int MotorOutput2;		  // 全局变量，RM电机输出
//函数声明
void GetMotorPulse(void);                     // 通过TIM3读取电机脉冲并计算速度
void SetMotorVoltageAndDirection(int Pwm);    // 电机电压和方向控制函数

#endif
