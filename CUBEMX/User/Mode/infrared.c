#include "infrared.h"
#include "usart.h" 
#include "main.h"

int out1,out2,out3,out4,out5,out6,angle; //��·����ȫ�ֱ���������Ƕ������
unsigned int state,Idata;     //Ѳ��״̬�����⺯���������

unsigned int InfraredScan()
{
	
	unsigned int state=0;
	
	out1 = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2);
	out2 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4);
	out3 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5);
	out4 = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4);
    out5 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11);
    out6 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10);//ȡ����·���������
	
	if(out1 == 1){state = 1;}//IN1��4����ת��
	if(out2 == 1){state = 2;}//IN2��2����ת��
	if(out3 == 1){state = 4;}//IN3��ֱ��ѭ��������
	if(out4 == 1){state = 8;}//IN4��ֱ��ѭ��������
    if(out6 == 1){state = 16;}//IN5��2����ת��
    if(out5 == 1){state = 32;}//IN6��4����ת��

//	if(out3==1&&out2==1){state = 6;}//1����ת��
//	if(out5==1&&out4==1){state = 24;}//1����Сת��
//
//    if(out2==1&&out1==1){state = 3;}//3����ת��
//    if(out6==1&&out5==1){state = 48;}//3����ת��
	if(out1==1&&out2==1&&out3==1&&out4==1){state = 0x0f;}//С������
    if(out1==0&&out2==0&&out3==0&&out4==0){state = 0;}//�հ�����

	return state;
	
}	
