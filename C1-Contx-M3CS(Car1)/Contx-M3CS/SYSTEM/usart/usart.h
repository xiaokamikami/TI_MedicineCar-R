#ifndef __USART_H
#define __USART_H
#include "stdio.h" 
#include "sys.h" 

#define DMA_Rec_Len 100      //����һ������Ϊ200���ֽڵ����ݻ������������鶨��ĳ��ȱ�����ܽ��յ������֡���ݳ��ȳ�����
extern __IO int k2_num ;   //��ȡͼ��
extern __IO int k2_tag_x  ;   //ָʾĿ��
//extern __IO int k2_val ;   //ͼ���С��Ͼ��� 
extern __IO u8 BALL_RX_FLAG;
//extern char* k2_x;   //����ֵ��� 
//extern char* k2_y;   //
//extern char* k2_color;   //��ɫ���� 

void uart_init(u32 bound);

#endif

