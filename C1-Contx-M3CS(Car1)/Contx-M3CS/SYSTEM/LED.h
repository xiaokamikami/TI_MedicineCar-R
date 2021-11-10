#ifndef _LED_H_
#define _LED_H_
#include "sys.h"  
#define R GPIO_Pin_11
#define G GPIO_Pin_10
#define B GPIO_Pin_13

#define KEK_T GPIO_Pin_11
#define KEK_R GPIO_Pin_10		//Right ÓÒ
#define KEK_L GPIO_Pin_13		//Left	×ó

void LED_GPIO_Init (void);
void LED_RGB(const char Color);

void KEY_GPIO_Init (void);
char KEY_Read (void);

#endif
