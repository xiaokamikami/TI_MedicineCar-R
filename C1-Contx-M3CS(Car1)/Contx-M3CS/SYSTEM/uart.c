#include "stm32f10x.h"
#include "main.h"

#include "usart.h"

#include "cJson.h"
#include "string.h"
#include "delay.h"
#include "stdlib.h"
extern __IO u8 sys_mode;
//static volatile	char *commonend;   //ָ����

void uart_init(u32 bound);
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);
u8 DMA_Rece_Buf[100];
u8 Print_Buf[50];
static void Json_RX(char * out);
char *Json_ptr;		  //Json�ַ�

__IO int k2_num = 0;   //��ȡͼ��
__IO int k2_tag_x  = 0;   //ָʾĿ��
//__IO int k2_val = 0;   //ͼ���С��Ͼ��� 
__IO u8 BALL_RX_FLAG = 0;

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

};

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0){}//ѭ������,ֱ���������   
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 

//��������  Json_Extract
//���ߣ�    Kamimiaomiao
//���ڣ�    2021-6-13
//���ܣ�    ��ȡJson��ʽ����
//���������AT͸�����յ�����      
//����ֵ��  ͸�����յ���Json����
//�޸ļ�¼��
//ʹ�÷���������͸�������ݷ���͸������Json��ʽ���ֵ��ַ���	   

static char* Json_Extract(char * Json_ptr ,char * Rxb)			//��ȡJsonָ��
{
	u8 i = 0;
	Json_ptr = strchr(Rxb,'{');
	for(i=0;i<sizeof(Json_ptr);i++)
	{
		if(Json_ptr[i] == '}')
		{
			Json_ptr[i+1] = 0;
			break;
		}
	}
	
	return  Json_ptr;
}

//��������  Json_RX
//���ߣ�    Kamimiaomiao
//���ڣ�    2021-6-13
//���ܣ�    �������ص�Json����
//���������Json��ʽ����      
//����ֵ�� //��ֱ�Ӵ���ָ���Ӧ�ı����� 

//�޸ļ�¼��
//ʹ�÷������ڳ�ʼ��ָ��������json_��������	   
//      	Ȼ��json_�������� = cJSON_GetObjectItem( json ,"��������");
//          ���������Լ�Ҫ���� Ĭ������ʾ����Ļ��
static void Json_RX(char * out)			//����Jsonָ��
{
    cJSON *json,*json_num,*json_tag_x;
    //out="{\"one\":\"long\",\"two\":\"2\",\"three\":3}"; //JSON��ʽʾ��
	
//	char commend1[10];
//	commonend = commend1;
	
    json = cJSON_Parse(out); //���õ����ַ���������json��ʽ
    json_num  = cJSON_GetObjectItem( json ,"n");  //��json��ȡ��ֵ����
	json_tag_x  = cJSON_GetObjectItem( json ,"x");//��json��ȡ��ֵ����
//	json_va = cJSON_GetObjectItem( json ,"v");//��json��ȡ��ֵ����
//  sprintf((char *)TXbuf,"\r\n one:%s   two:%lf   three:%d",json_one->valuestring,json_two->valuedouble,json_three->valueint);
//**������1
//	strcpy((char *)commonend, json_x->valuestring);
//	k2_x  = atoi((char *)commonend);
//	strcpy((char *)commonend, json_y->valuestring);
//	k2_y  = atoi((char *)commonend);
//	strcpy((char *)commonend, json_co->valuestring);
//	k2_color  = atoi((char *)commonend);	
//	printf("comm:%s\n",commonend);
	
//**������2
	k2_num = json_num->valueint;	
	k2_tag_x = json_tag_x->valueint;	
//	k2_val = json_va->valueint;
	//printf("k2:%d",k2_x);



//	sprintf((char *)TXbuf,"MQTT:command:%s \r\n ",json_command->valuestring);	//���ڷ���ָ��		
//	HAL_UART_Transmit(&huart1, (uint8_t *)TXbuf, sizeof(TXbuf),200);
//	memset((char *)TXbuf, 0, sizeof(TXbuf)); 
	
	delay_ms(10);
    cJSON_Delete(json);  //�ͷ��ڴ� 
//	return json_command->valuestring;
} 

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

   RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_GPIOA,ENABLE); //ʹ��USART1��GPIOAʱ��
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //ʹ��DMA����
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��

   USART_DeInit(USART2);  //��λ����1
   //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������
  USART_InitStructure.USART_BaudRate = bound;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
  USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;//��Ӳ������������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//���������ж�
    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);   //ʹ�ܴ���1 DMA����
    USART_Cmd(USART2, ENABLE);                   //ʹ�ܴ��� 
 
    //��Ӧ��DMA����
  DMA_DeInit(DMA1_Channel6);   //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ  ����1��Ӧ����DMAͨ��5
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR; //DMA����usart����ַ
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)DMA_Rece_Buf;  //DMA�ڴ����ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴������ȡ���͵��ڴ�
  DMA_InitStructure.DMA_BufferSize = DMA_Rec_Len;  //DMAͨ����DMA����Ĵ�С
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�Ĵ�������
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //���ݿ��Ϊ8λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��

    DMA_Cmd(DMA1_Channel6, ENABLE);  //��ʽ����DMA����
}

//���»ָ�DMAָ��
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //�ر�USART1 TX DMA1��ָʾ��ͨ��    
 	DMA_SetCurrDataCounter(DMA_CHx,DMA_Rec_Len);//DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA_CHx, ENABLE);  //��USART1 TX DMA1��ָʾ��ͨ��  
}	

//����len���ֽ�
//buf:�������׵�ַ
//len:���͵��ֽ���
void Usart2_Send(u8 *buf,u8 len)
{
	u8 t;
  	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART2,buf[t]);
	}	 
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		
}

//�����жϺ���
void USART2_IRQHandler(void)                //����1�жϷ������
{
	static u16 Usart2_Rec_Cnt;
     if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
      {
          USART_ReceiveData(USART2);//��ȡ����ע�⣺������Ҫ�������ܹ�����жϱ�־λ��
		  
			if(BALL_RX_FLAG == 0)
			{
				Usart2_Rec_Cnt =DMA_Rec_Len-DMA_GetCurrDataCounter(DMA1_Channel6); //����ӱ�֡���ݳ���
	   
			 //***********֡���ݴ�����************//
				Json_RX((char * )DMA_Rece_Buf) ;
				//printf("x:%d,y:%d,color:%d \n",k2_x,k2_y,k2_color);
				BALL_RX_FLAG = 1;		
			  }

        //*************************************//
         USART_ClearITPendingBit(USART2,USART_IT_IDLE);         //����жϱ�־
         MYDMA_Enable(DMA1_Channel6);                  //�ָ�DMAָ�룬�ȴ���һ�εĽ���
     } 

}



