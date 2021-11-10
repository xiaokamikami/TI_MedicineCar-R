#ifndef __INFRARED_H
#define __INFRARED_H

extern int out1,out2,out3,out4,out5,out6,angle;//四路红外数据，舵机角度输出;
extern unsigned int state,Idata;//巡线状态，红外函数输出数据;

unsigned int InfraredScan(void);//声明红外扫描函数

#endif
