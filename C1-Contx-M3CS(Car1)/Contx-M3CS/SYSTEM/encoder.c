#include "encoder.h"
#include "main.h"
#include "math.h"
#include "stdlib.h"
/**************************************************************************
�������ܣ���TIM3��ʼ��Ϊ�������ӿ�ģʽ
��ڲ�������
����  ֵ����
**************************************************************************/
void Encoder_Init_TIM3(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    //ʹ�ܶ�ʱ��3��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //ʹ��PA�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6;	//�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING;   //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//�����趨������ʼ��GPIOA

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;              //Ԥ��Ƶ�� 
	TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;  //�趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3
	
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 10;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);                   //���TIM�ĸ��±�־λ
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	//Reset counter
	TIM_SetCounter(TIM3,0);
	//===============================================
	TIM3->CNT = 0x7fff;
	//===============================================
	TIM_Cmd(TIM3, ENABLE); 
}
/**************************************************************************
�������ܣ���TIM5��ʼ��Ϊ�������ӿ�ģʽ
��ڲ�������
����  ֵ����
**************************************************************************/
void Encoder_Init_TIM5(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);    //ʹ�ܶ�ʱ��4��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //ʹ��PB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_0;	//�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//�����趨������ʼ��GPIOB

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;              // Ԥ��Ƶ�� 
	TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;  //�趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���  
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 10;
	TIM_ICInit(TIM5, &TIM_ICInitStructure);
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);                   //���TIM�ĸ��±�־λ
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	//Reset counter
	TIM_SetCounter(TIM5,0);
	//===============================================
	TIM5->CNT = 0x7fff;
	//===============================================
	TIM_Cmd(TIM5, ENABLE); 
}

/**************************************************************************
�������ܣ���ȡ�����������ֵ
��ڲ�����TIM_TypeDef * TIMx
����  ֵ����
**************************************************************************/
s16 getTIMx_DetaCnt(TIM_TypeDef * TIMx)
{
	s16 cnt;
	cnt = TIMx->CNT-0x7fff;
	TIMx->CNT = 0x7fff;
	return cnt;
}

/**************************************************************************
�������ܣ�������������
��ڲ�����int *leftSpeed,int *rightSpeed
����  ֵ����
		//�������ҳ������ٶȣ������ٶ�Ϊ��ֵ �������ٶ�Ϊ��ֵ���ٶ�Ϊ����1000֮����ٶ� mm/s
		//һ��ʱ���ڵı������仯ֵ*ת���ʣ�ת��Ϊֱ���ϵľ���m��*200s��5ms����һ�Σ� �õ� m/s *1000ת��Ϊint����

		һȦ����������
			��1560
			�ң�1040
		���Ӱ뾶��0.03m
		�����ܳ���2*pi*r
		һ������ľ��룺
			��0.000120830m
			�ң�0.000181245m
		�ٶȷֱ��ʣ�
			��0.0120m/s 12.0mm/s
			�ң�0.0181m/s 18.1mm/s
**************************************************************************/

int Get_Motor_Speed(int *leftSpeed,int *rightSpeed)
{

	static int leftWheelEncoderLast   = 0;
	static int rightWheelEncoderLast  = 0;	

	//��¼�������ұ���������

	
	leftWheelEncoderNow += (abs(getTIMx_DetaCnt(TIM3)));
	rightWheelEncoderNow+= (abs(getTIMx_DetaCnt(TIM5)));


	//10ms����    	
	*leftSpeed   = (leftWheelEncoderNow - leftWheelEncoderLast)*64; 
	*rightSpeed  = (rightWheelEncoderNow - rightWheelEncoderLast)*64;


	//��¼�ϴα���������
	leftWheelEncoderLast  = leftWheelEncoderNow;                    
	rightWheelEncoderLast = rightWheelEncoderNow;  
//	//return leftWheelEncoderNow,rightWheelEncoderNow;
	//*leftSpeed   = getTIMx_DetaCnt(TIM3)*1000*200*0.000120830;  
	//*rightSpeed  = getTIMx_DetaCnt(TIM5)*1000*200*0.000120830;

	return 0;
}
