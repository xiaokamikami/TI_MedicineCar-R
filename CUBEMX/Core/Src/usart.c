/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
#include "string.h"
uint8_t USART_TX_BUF[USART_TX_LEN];
uint8_t USART_RX_BUF[USART_RX_LEN];

/* USER CODE END 0 */

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart2_rx;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Channel7;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);

    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Channel6;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void Int_to_Byte(int i,uint8_t *byte)
{

    unsigned long longdata = 0;
    longdata = *(unsigned long*)&i;
    byte[3] = (longdata & 0xFF000000) >> 24;
    byte[2] = (longdata & 0x00FF0000) >> 16;
    byte[1] = (longdata & 0x0000FF00) >> 8;
    byte[0] = (longdata & 0x000000FF);

}
void Float_to_Byte(float f,uint8_t *byte)
{

    unsigned long longdata = 0;
    longdata = *(unsigned long*)&f;
    byte[3] = (longdata & 0xFF000000) >> 24;
    byte[2] = (longdata & 0x00FF0000) >> 16;
    byte[1] = (longdata & 0x0000FF00) >> 8;
    byte[0] = (longdata & 0x000000FF);

}

void Short_to_Byte(short s,uint8_t *byte)
{

    byte[1] = (s & 0xFF00) >> 8;
    byte[0] = (s & 0xFF);
}

//串口1中断服务程序
//Ble_Data rec_data;	//蓝牙接收结构体
//void USART3_IRQ_Cb(void)
//{
//    uint8_t err = 0;
//    if((__HAL_UART_GET_FLAG(&huart3,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x5a结尾0xa5开头)
//    {
//        HAL_UART_Receive(&huart3,USART_RX_BUF,USART_RX_LEN,100); //读取串口接收的一字节数据到Res

//        //检查包头，包尾
//        if(USART_RX_BUF[0]==0xa5 && USART_RX_BUF[USART_RX_LEN-1]==0x5a)
//        {
//            //检查校验和
//            err = (uint8_t) ((USART_RX_BUF[1]+USART_RX_BUF[2]+USART_RX_BUF[3]+USART_RX_BUF[4]+USART_RX_BUF[5]+USART_RX_BUF[6]+USART_RX_BUF[7]+USART_RX_BUF[8]+USART_RX_BUF[9]+USART_RX_BUF[10]+USART_RX_BUF[11])&0xff);
//            if(err!=USART_RX_BUF[12])
//            {
//                //返回错误 数据无效
//                rec_data.option |=0x00;
//                return ;
//            }
//            //数据有效
//            rec_data.option |=0x01;

//            //将buff中的字节数据转换成指定类型
//            rec_data.x = (char)USART_RX_BUF[1];
//            rec_data.y = *((short *) (&USART_RX_BUF[2]));
//            rec_data.z = *((int *) (&USART_RX_BUF[4]));
//            rec_data.f = *((float *) (&USART_RX_BUF[8]));

//            if(rec_data.x == 28)
//            {
////                CarStart=1;
//            }
//            if(rec_data.x == 36)
//            {
////                CarStart=0;
//            }
//        }
//    }

//}


void UART_RxIdleCallback(UART_HandleTypeDef *huart)
{
	 if(__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE)){
	 __HAL_UART_CLEAR_IDLEFLAG(huart);
	 
	 
	 HAL_UART_DMAStop(huart);
	 
	 //hdma_usart2_rx.Instance->CNDTR = BUFFER_SIZE;
	 //rxSize = RX_SIZE-hdma_usart2_rx.Instance->CNDTR;
	 //printf("uart2 receive CNT: %d\n",rxSize);
		 printf("data:%s\n",USART_RX_BUF);
		 memset(USART_RX_BUF,0,sizeof(USART_RX_BUF));
		 HAL_UART_Receive_DMA(huart,USART_RX_BUF,USART_RX_LEN);
 
 }
 }


////send data to hc-05
//int ble_send(uint8_t *data,int len)
//{
//    while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)!=SET);	//wait until tx complete
//    if(!data)return -1;
//    HAL_UART_Transmit_DMA(&huart3,data,len);	//使用DMA模式发送
//    while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)!=SET);
//    return 0;
//}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
