#include "Button.h"
#include "main.h"
#include "tim.h"
#include "control.h"
#include "stm32f1xx_it.h"

int Count;//i����int�ͱ����� ButtonCount��ʾ������������
int Flag;//i����int�ͱ����� ButtonFlag��ʾ�ذ�����־�� 1�������°����� 0Ϊû�����°���
int ButtonState;//g��globle����ȫ�ֱ����� ���������ط����ã� i����int�ͱ����� ButtonState��ʾ������־�� 1�����£� 0�����ɿ�

void ButtonScan(void)
{
	if( HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == GPIO_PIN_SET )//������ż�⵽�ߵ�ƽ
	{
		Count++; //�������£� ����iButtonCount��1
		if(Count >= 30) //1ms�жϷ�����������һ�Σ� iButtonCount���ڵ���30�� ���������ȶ�����30ms
		{
			
			if(Flag == 0) //�ж���û���ذ����� 1Ϊ�У� 0Ϊû��
			{
				
				if(ButtonState == 0)  ButtonState = 1; //���ð�����־
				else                  ButtonState = 0;//���º�Ϊ 1 ��Ϊ 0��1������0ֹͣ
				
				Count = 0;
				Flag = 1; //�����ذ�����־
				
			}
			else Count = 0;//����ذ����� �����¼���
			
		}
//		else ButtonState=0;//���û���ȶ�����30ms�� �����û�а��°���
		
	}
	else //���һֱ�޼�⵽�ߵ�ƽ�� ��һֱ�ް�������
	{
		Count = 0; //����iButtonCount
//		ButtonState=0; //���������־
		Flag = 0; //����ذ�����־
	}
	
	if(ButtonState == 0)
	{
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1,0); 
		__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);//A���
	
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3,0); 
		__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_4,0);//B���
	}
	
}
