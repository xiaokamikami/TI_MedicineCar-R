#include "control.h"
#include "tim.h"
#include "main.h"
#include "math.h"
#include "stdlib.h"
int MotorSpeed1 = 300;  // ȫ�ֱ�����LM�����ǰ�ٶ���ֵ���ӱ������л�ȡ
int MotorSpeed2 = 300;  // ȫ�ֱ�����RM�����ǰ�ٶ���ֵ���ӱ������л�ȡ
int SpeedTarget1 =300;	  // ȫ�ֱ�����LM�ٶ�Ŀ��ֵ
int SpeedTarget2 =300;	  // ȫ�ֱ�����RM�ٶ�Ŀ��ֵ
int MotorOutput1 =300;		  // ȫ�ֱ�����LM������
int MotorOutput2 =300;		  // ȫ�ֱ�����RM������

// 1.ͨ��TIM3��ȡ������岢�����ٶ�
void GetMotorPulse(void)
{
  // ��������õ�����壬�õ����10ms����������/������ת����Ϊʵ��ת�ٵ�rpm
  MotorSpeed1 = (short)(__HAL_TIM_GET_COUNTER(&htim3)); 
  MotorSpeed2 = (short)(__HAL_TIM_GET_COUNTER(&htim4)); 

  if(MotorSpeed1 < 0 )
  {
	   // MotorSpeed1 +=26549;
	    MotorSpeed1 = (abs(MotorSpeed1)*180);

  }
  else{MotorSpeed1 = (abs(MotorSpeed1)*180);}
   MotorSpeed1 /= 6;
  //MotorSpeed = (short)(__HAL_TIM_GET_COUNTER(&htim3));
  __HAL_TIM_SET_COUNTER(&htim3,0);  // ����������
  
   if(MotorSpeed2 < 0 )
  {
	   // MotorSpeed2 +=26549;
	    MotorSpeed2= (abs(MotorSpeed2)*180);

  }
  else{MotorSpeed2 = (abs(MotorSpeed2)*180);}
  MotorSpeed2 /= 6;
  //MotorSpeed = (short)(__HAL_TIM_GET_COUNTER(&htim3));
  __HAL_TIM_SET_COUNTER(&htim4,0);  // ����������

}

