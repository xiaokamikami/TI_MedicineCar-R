#ifndef __CONTROL_H
#define __CONTROL_H
#include "main.h"
//ȫ�ֱ���


extern int MotorOutput1;		  // ȫ�ֱ�����LM������
extern int MotorOutput2;		  // ȫ�ֱ�����RM������
//��������
void GetMotorPulse(void);                     // ͨ��TIM3��ȡ������岢�����ٶ�
void SetMotorVoltageAndDirection(int Pwm);    // �����ѹ�ͷ�����ƺ���
void Motor_X(uint8_t mode);
#endif
