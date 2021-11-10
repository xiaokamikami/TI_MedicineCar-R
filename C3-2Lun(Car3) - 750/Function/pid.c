#include "pid.h"
// 2.����ʽPID������
int Error_Last1=0,Error_Prev1=0;          // ��һ��ƫ��ֵ�����ϴ����
int Error_Last2=0,Error_Prev2=0;          // ��һ��ƫ��ֵ�����ϴ����
int Pwm_add1 = 70,Pwm1 =70,Pwm_last1 = 70;                    // PWM������PWM���ռ�ձ�
int Pwm_add2 = 70,Pwm2 =70,Pwm_last2 = 70;                    // PWM������PWM���ռ�ձ�
float Kp = 0.3	, Ki = 0.1, Kd = 0.08;       // PIDϵ��������ֻ�õ�PI������

int SpeedInnerControl1(int Speed,int Target) // �ٶ��ڻ�����
{
    int Error = Target - Speed ;		  // ƫ�� = Ŀ���ٶ� - ʵ���ٶ�
//	if(-20.0 < Error < 20.0){
//		Pwm = Pwm_last;
//		return Pwm;
//	}
	if(Target == 0){
		Error_Last1=0,Error_Prev1=0;
	}	
    Pwm_add1 = Kp * (Error - Error_Last1) + 					  // ����
			  Ki * Error +							    	  // ����
			  Kd * (Error - 2.0f * Error_Last1 + Error_Prev1)	  // ΢��
              ;  // ��һ��Ŀ�����������ź�Ϊ0ʱ��ϵͳ������ʧ��״̬
	
    Pwm1 = Pwm_add1+Pwm_last1;		              // ԭʼ��+���� = �����
	Pwm_last1 = Pwm1;					  //�����ϴ�PWMֵ
	Error_Prev1 = Error_Last1;	  	  // �������ϴ����
    Error_Last1 = Error;	              // �����ϴ�ƫ��

    if(Pwm1 >= 70) Pwm1 = 70;	      // �޷�
    if(Pwm1 <= 50) Pwm1 = 50;
    return Pwm1;	                      // �������ֵ
} 

int SpeedInnerControl2(int Speed,int Target) // �ٶ��ڻ�����
{
    int Error = Target-Speed;		  // ƫ�� = Ŀ���ٶ� - ʵ���ٶ�
//	if(-20.0 < Error < 20.0){
//		Pwm = Pwm_last;
//		return Pwm;
//	}
	if(Target == 0){
		Error_Last2=0,Error_Prev2=0;
	}	
    Pwm_add2 = Kp * (Error - Error_Last2) + 					  // ����
			  Ki * Error +							    	  // ����
			  Kd * (Error - 2.0f * Error_Last2 + Error_Prev2)	  // ΢��
              ;  // ��һ��Ŀ�����������ź�Ϊ0ʱ��ϵͳ������ʧ��״̬
	
    Pwm2 = Pwm_add2+Pwm_last2;		              // ԭʼ��+���� = �����
	Pwm_last2 = Pwm2;					  //�����ϴ�PWMֵ
	Error_Prev2 = Error_Last2;	  	  // �������ϴ����
    Error_Last2 = Error;	              // �����ϴ�ƫ��

    if(Pwm2 >= 70) Pwm2 = 70;	      // �޷�
    if(Pwm2 <= 50) Pwm2 = 50;
    return Pwm2;	                      // �������ֵ
} 
