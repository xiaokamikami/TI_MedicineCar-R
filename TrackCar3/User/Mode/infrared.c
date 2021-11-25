#include "infrared.h"
#include "usart.h" 
#include "main.h"

int out1,out2,out3,out4,out5,out6,angle; //��·����ȫ�ֱ���������Ƕ������
unsigned int state,Idata;     //Ѳ��״̬�����⺯���������

unsigned int InfraredScan()
{
	
	unsigned int state=0;
	
	out1 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14);
	out2 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15);
	out3 = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8);
	out4 = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9);
    out5 = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_10);
    out6 = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_11);//ȡ����·���������
	
	if(out1 == 1){state = 1;}//IN1��4����ת��
	else if(out2 == 1){state = 2;}//IN2��2����ת��
	if(out3 == 1){state = 4;}//IN3��ֱ��ѭ��������
	else if(out4 == 1){state = 8;}//IN4��ֱ��ѭ��������
    if(out5 == 1){state = 16;}//IN5��2����ת��
    else if(out6 == 1){state = 32;}//IN6��4����ת��

//	if(out3==1&&out2==1){state = 6;}//1����ת��
//	if(out5==1&&out4==1){state = 24;}//1����Сת��
//
//    if(out2==1&&out1==1){state = 3;}//3����ת��
//    if(out6==1&&out5==1){state = 48;}//3����ת��
    Idata = out1+out2+out3+out4+out5+out6;
	if(out1==1&&out2==1&&out3==1&&out4==1){state = 0x0f;}//С������
    else if(out1==0&&out2==0&&out3==0&&out4==0){state = 0;}//�հ�����
	else if(Idata > 2){state = 0x0d;}
	return state;
	
}	
