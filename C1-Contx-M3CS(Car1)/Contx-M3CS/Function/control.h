#ifndef __CONTROL_H
#define __CONTROL_H


//ȫ�ֱ���
extern int MotorSpeed1;  // ȫ�ֱ�����LM�����ǰ�ٶ���ֵ���ӱ������л�ȡ
extern int MotorSpeed2;  // ȫ�ֱ�����RM�����ǰ�ٶ���ֵ���ӱ������л�ȡ
extern int SpeedTarget1;	  // ȫ�ֱ�����LM�ٶ�Ŀ��ֵ
extern int SpeedTarget2;	  // ȫ�ֱ�����RM�ٶ�Ŀ��ֵ
extern int MotorOutput1;		  // ȫ�ֱ�����LM������
extern int MotorOutput2;		  // ȫ�ֱ�����RM������
//��������
void GetMotorPulse(void);                     // ͨ��TIM3��ȡ������岢�����ٶ�
void SetMotorVoltageAndDirection(int Pwm);    // �����ѹ�ͷ�����ƺ���

#endif
