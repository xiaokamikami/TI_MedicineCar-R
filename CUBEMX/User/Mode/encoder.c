#include "tim.h"//����timͷ�ļ�
#include "encoder.h"

int AEncoder,BEncoder;//��Ŵ�TIM4��ʱ���������ı���������
int GetTim4Encoder(void)//��ȡTIM4��ʱ���������ı���������
{
	
	BEncoder = (short)(__HAL_TIM_GET_COUNTER(&htim3));//�ȶ�ȡ������
	__HAL_TIM_SET_COUNTER(&htim3,0);//�ټ���������
	
	return BEncoder;//����������
	
}

int GetTim2Encoder(void)//��ȡTIM2��ʱ���������ı���������
{
	
	AEncoder = (short)(__HAL_TIM_GET_COUNTER(&htim5));//�ȶ�ȡ������
	__HAL_TIM_SET_COUNTER(&htim5,0);//�ټ���������
	
	return AEncoder;//����������
	
}
