#ifndef __USART_H
#define __USART_H
#include "stdio.h" 
#include "sys.h" 

#define DMA_Rec_Len 100      //定义一个长度为200个字节的数据缓冲区。（建议定义的长度比你可能接收到的最长单帧数据长度长！）
extern __IO int k2_num ;   //获取图像
extern __IO int k2_tag_x  ;   //指示目标
//extern __IO int k2_val ;   //图像大小拟合距离 
extern __IO u8 BALL_RX_FLAG;
//extern char* k2_x;   //坐标值存放 
//extern char* k2_y;   //
//extern char* k2_color;   //颜色数据 

void uart_init(u32 bound);

#endif

