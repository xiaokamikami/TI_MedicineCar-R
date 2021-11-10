/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#define USART_TX_LEN 14
extern uint8_t USART_TX_BUF[USART_TX_LEN];
#define USART_RX_LEN 14
extern uint8_t USART_RX_BUF[USART_RX_LEN];
typedef struct {
    uint8_t option;	//bit0 indecate weather the msg is rec
    char x;
    short y;
    int z;
    float f;
}Ble_Data;

extern Ble_Data rec_data;
/* USER CODE END Includes */

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void USART3_IRQ_Cb(void);

void Int_to_Byte(int i,uint8_t *byte);
void Float_to_Byte(float f,uint8_t *byte);
void Short_to_Byte(short s,uint8_t *byte);
int ble_send(uint8_t *data,int len);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
