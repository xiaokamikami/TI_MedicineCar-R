#ifndef _GPIO_H
#define _GPIO_H

#define RES1	GPIO_Pin_2 //D2
#define RES2	GPIO_Pin_4	//B4
#define RES3	GPIO_Pin_5
#define RES4	GPIO_Pin_4 //A4

#define RES5	GPIO_Pin_11
#define RES6	GPIO_Pin_10

#define RES_G	GPIO_Pin_2
#include "stm32f10x.h"

void RES_Init(void);


#endif

