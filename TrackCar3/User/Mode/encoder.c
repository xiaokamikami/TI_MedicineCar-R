#include "tim.h"//����timͷ�ļ�
#include "encoder.h"

int AEncoder,BEncoder;//��Ŵ�TIM4��ʱ���������ı���������
int GetTim4Encoder(void)//��ȡTIM4��ʱ���������ı���������
{
	
	BEncoder = ((short)(__HAL_TIM_GET_COUNTER(&htim2)));//�ȶ�ȡ������
	__HAL_TIM_SET_COUNTER(&htim2,0);//�ټ���������
    //BEncoder = BEncoder*-1;
	return BEncoder;//����������
	
}

int GetTim2Encoder(void)//��ȡTIM2��ʱ���������ı���������
{
	
	AEncoder = ((short)(__HAL_TIM_GET_COUNTER(&htim4)));//�ȶ�ȡ������
	__HAL_TIM_SET_COUNTER(&htim4,0);//�ټ���������
    //AEncoder = AEncoder*-1;
	return AEncoder;//����������
	
}
