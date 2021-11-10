/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "control.h"
#include "OpenmvData.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//系统运行时间计数，，开机后开始计数，每秒增1
unsigned short  g_RunTime = 0;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
void SecTask();
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    static uint8_t Lock_Phoht = 0; //锁定次数
    static uint8_t Lock_Num = 0; //
    static uint8_t Lock_Tag = 0; //
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  mystdioInit(&huart1);//printf初始化

  HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);


  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1); //一定要手动开启！！！！
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2); //一定要手动开启！！！！
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3); //打开TIM3的四个通道
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4); //一定要手动开启！！！！

  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);//打开舵机PWM通道
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);//打开相机PWM通道
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);//开启TIM4的编码器接口模式
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);//开启TIM2的编码器接口模式
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 75);//输出小车角度值
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 65);//输出相机角度值  默认看右
  //HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断

//      SpeedTarget = 200;                              //速度设置（巡线要快 抢时间）

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

//              __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 45);//输出小车角度值
//              __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 76);//输出相机角度值     左
//      HAL_Delay(1000);
//              __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 75);//输出小车角度值     中
//              __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 65);//输出相机角度值
//      HAL_Delay(1000);
//              __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 100);//输出小车角度值
//              __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 65);//输出相机角度值     右
//      HAL_Delay(1000);
      if(OpenmvData())                         //判断串口（摄像头检测到那个数字）
      {
/*          if(UART2_Rx_flg == 1)
          {
              if(Lock_Phoht  != Xdate)      //连续三次数据一样确认数据
              {
                  Lock_Phoht  = Xdate;
                  Lock_Num = 0;
              }
              else if(Lock_Phoht == Xdate)      //连续三次数据一样确认数据
              {
                  Lock_Num +=1;
              }
              if(Lock_Num >3)
              {
              */
                  Lock_Tag = Xdate;
//             }
              UART2_Rx_cnt = 0;
              UART2_Rx_flg=0;
//           }

          HAL_Delay(500);
          switch (Lock_Tag)                //输入数字进行选择
          {
              case 1:
                  BaseOne();        //数字一       && 同时检测药物是否装上
              case 2:
                  BaseTwo();        //数字二
              case 3:
                  BaseThree();      //数字    //345678通用代码
              case 4:
                  BaseThree();      //数字
              case 5:
                  BaseThree();      //数字
              case 6:
                  BaseThree();      //数字
              case 7:
                  BaseThree();      //数字
              case 8:
                  BaseThree();      //数字
          }
      }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
