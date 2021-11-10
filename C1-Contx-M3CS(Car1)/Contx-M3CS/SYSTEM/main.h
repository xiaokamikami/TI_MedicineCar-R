#ifndef __MAIN_H
#define	__MAIN_H
#include "stm32f10x.h"
#include <stdio.h>
#include <math.h>
extern __IO u8 Key1_num ,sys_mode,Key3_num ,Res_near ;
extern int leftWheelEncoderNow ;
extern int rightWheelEncoderNow;

extern u8 time_10ms ;
extern u8 time_20ms ;
extern u8 time_50ms ;

#endif /* __MAIN_H */
