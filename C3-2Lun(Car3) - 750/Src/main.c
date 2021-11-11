/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "pid.h"
#include "control.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "mpu6050.h"
#include "inv_mpu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//****���ڱ���***
static uint8_t recv_end_flag = 0 ; //���ڽ��ձ�־
//********
int Cama_X =0;	  		// ȫ�ֱ��������X��
int Cama_N =0;	  		// ȫ�ֱ����������ȡ����
static char xx[5],yy[5];
static float Gyro[3];

static uint8_t lcdbuff[20]; //UART�ַ�
static char uart1_buff[20]; //UART�ַ�

static unsigned char UART2_Rx_Buf[512] = {0}; //USART2�洢��������
static unsigned char UART2_Rx_flg = 0;                   //USART2������ɱ�־
static unsigned int  UART2_Rx_cnt = 0;                   //USART2�������ݼ�����
static unsigned char UART2_temp[1] = {0};       //USART2�������ݻ���

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

static void Motor_Locat(int SpeedTarget1,int SpeedTarget2){
	
}
static void DockV(uint16_t Dock1,uint16_t Dock2){
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,Dock1);//������
	
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,Dock2);//������
	
	HAL_Delay(100);
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();
	
	/* Configure the MPU attributes for the QSPI 256MB without instruction access */
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress      = QSPI_BASE;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_256MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
  /* Configure the MPU attributes for the QSPI 8MB (QSPI Flash Size) to Cacheable WT */
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress      = QSPI_BASE;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_8MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

static void LED_Blink(uint32_t Hdelay,uint32_t Ldelay)
{
	HAL_GPIO_WritePin(E3_GPIO_Port,E3_Pin,GPIO_PIN_SET);
	HAL_Delay(Hdelay - 1);
	HAL_GPIO_WritePin(E3_GPIO_Port,E3_Pin,GPIO_PIN_RESET);
	HAL_Delay(Ldelay-1);
}

/**
  * @brief  Get the current time and date.
  * @param  
  * @retval None
  */
static void RTC_CalendarShow(RTC_DateTypeDef *sdatestructureget,RTC_TimeTypeDef *stimestructureget)
{
  /* ����ͬʱ��ȡʱ������� ��Ȼ�ᵼ���´�RTC���ܶ�ȡ */
  /* Both time and date must be obtained or RTC cannot be read next time */
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, sdatestructureget, RTC_FORMAT_BIN);
}
#pragma import(__use_no_semihosting)
/* ���HAL��ʹ��ʱ, ĳЩ������ܱ�����bug */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* ���HAL��ʹ��ʱ, ĳЩ������ܱ�����bug */
struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

/* FILE is typedef�� d in stdio.h. */
FILE __stdout;

/* ����_sys_exit()�Ա���ʹ�ð�����ģʽ */
void _sys_exit(int x)
{
    x = x;
}
/* �ض���fputc����, printf�������ջ�ͨ������fputc����ַ��������� */
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1 , (uint8_t *)&ch, 1, 0xFFF);
    return ch;
}

/*
��ȡ����ͷֵ
�÷�: K210�򴮿ڷ��� [xxx,yyy]  X������ͷ��ȡ���ĺ�����  Y���������
*/
u8 K210_Read(void )
{
	int x=0,y=0,z=0,t=0;
    if(UART2_Rx_flg)//������ɱ�־���ж��Ƿ���������ݣ�
    {
        printf("--------2-------\n");
        for (t = 0; t < UART2_Rx_cnt; t++)//ͨ��Uart2�������ݼ�������ȷ������������λ
        {
            /* ԭʼ����Ϊ��[xxx,yyy] �˶�Ϊȥ����[����,����]���������ţ�����x,y������*/
            if (UART2_Rx_Buf[t] == '[')x = 1, y = 0;
            if (UART2_Rx_Buf[t] == ',') {
                x = 0, y = 1;
                xx[t - 1] = '\0';
            }
            if (UART2_Rx_Buf[t] == ']') {
                yy[z] = '\0';
                x = 0, y = 0, z = 0;
            }
            if (x == 1 && UART2_Rx_Buf[t] != '[')xx[t - 1] = UART2_Rx_Buf[t];
            if (y == 1 && UART2_Rx_Buf[t] != ',') {
                yy[z] = UART2_Rx_Buf[t];
                z++;
            }
        }

        Cama_X = atoi(&xx[0]);//(atoi():��һ�����ַ���ת��Ϊ���͵ĺ���)
        Cama_N = atoi(&yy[0]);
        printf("X=%d\n", Cama_X);//��Uart1Ϊͨ��ͨ���ض�����printf()��ӡx,y����
        printf("Num=%d\n", Cama_N);
	}
}

/*
��ֱ��Ѳ��	ѭ����  
����:�ú�����ʹС������������ֱ����
����ֵ:��ȡ���ĺ�/��������
*/
u8 Lock_Line(void )
{
	u8 Res_n = 0;
	u8 SP_differential = 0;  //����ֵ����
	Res_n = 0;
	
	if(HAL_GPIO_ReadPin(RE1_GPIO_Port,RE1_Pin)==1)	{SP_differential =40;Res_n+=1;}		//��ƫ��λ
	if(HAL_GPIO_ReadPin(RE2_GPIO_Port,RE2_Pin)==1)	{SP_differential =30;Res_n+=1;}	//��ƫ
	if(HAL_GPIO_ReadPin(RE3_GPIO_Port,RE3_Pin)==1)	{SP_differential =10;Res_n+=1;}	//Ѳ����λ
	if(HAL_GPIO_ReadPin(RE4_GPIO_Port,RE4_Pin)==1)	{SP_differential =0;Res_n+=1;}	//Ѳ����λ
	if(HAL_GPIO_ReadPin(RE5_GPIO_Port,RE5_Pin)==1)	{SP_differential =-10;Res_n+=1;}	//��ƫ	
	if(HAL_GPIO_ReadPin(RE6_GPIO_Port,RE6_Pin)==1)	{SP_differential =-30;Res_n+=1;}		//��ƫ��
	

	if(Res_n == 0)
	{return Res_n;}//û��鵽�� ����
	//���� ������ٿ��ƶ�
	else if(Res_n == 1)
	{
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,280+SP_differential);//ת�併���ٶ�ת
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,280-SP_differential);//
	}
	else if(Res_n > 1)	//�⵽������ ͣ��
	{
		Motor_X(0);
	}
	return Res_n;
}


/* 
����:ת��90��		����ǰ5��ͣ�·�ֹ����
ʹ�÷�ʽ:1��2��
*/
void Turn_Set(uint8_t Mode)
{
	uint16_t Z_Turn = 0;
	float Z_Angle = 0;
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,280);//ת�併���ٶ�ת
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,280);//
	if(Mode == 2)
	{
			Motor_X(0);
			//*** ��ת90 ��  MPUӦ�ü�С90       С��90�ĳ�ʼֵ  (Խ0��360)ת��Ŀ��ֵ��LOCK+270  //LOCK+270 = 360+LOCK-90
			mpu_dmp_get_data(&Gyro[2]);  
			Z_Angle = Gyro[2];
			Motor_X(4);
			if(Z_Angle <=91)
			{
				Z_Turn = Z_Angle +275;
				//�����ж�
				R1:
				if(Gyro[2] >180)
				{
					while(1)
					{		 
						//mpu_dmp_get_data(&Gyro[2]);
						//printf("RRRF:LOCK=%.2f,Z=%.2f,TAG:%d \r\n",Z_Angle,Gyro[2],Z_Turn);
						if(Gyro[2] < Z_Turn)
						{
							Motor_X(0);
							printf("R_END");
							return;
						}
					}					
				}
				else
				{
					//printf("RRF:LOCK=%.2f,Z=%.2f,TAG:%d \r\n",Z_Angle,Gyro[2],Z_Turn);	
					//mpu_dmp_get_data(&Gyro[2]);
					goto R1;
				}
					

			}
			else
			{
				Z_Turn = Z_Angle -85;
				while(1)
				{		 
					//mpu_dmp_get_data(&Gyro[2]);
					//printf("RF:LOCK=%.2f,Z=%.2f,TAG:%d \r\n",Z_Angle,Gyro[2],Z_Turn);
					if(Gyro[2] < Z_Turn )
					{
						Motor_X(0);
						printf("R_END");
						return;
					}
				}
			}	
	}
	else if(Mode == 1)
	{
			Motor_X(0);
			//*** ��ת90 ��  MPUӦ������90   
			mpu_dmp_get_data(&Gyro[2]);
			Z_Angle = Gyro[2];
			Motor_X(3);
			if(Z_Angle >=271)
			{
				Z_Turn = Z_Angle -275;
				//�����ж�
				L1:
				if(Gyro[2] < 180)
				{
					while(1 )
					{
						//mpu_dmp_get_data(&Gyro[2]);
						//printf("LLLF:LOCK=%.2f,Z=%.2f,TAG:%d \r\n",Z_Angle,Gyro[2],Z_Turn);	
						if(Gyro[2] > Z_Turn)
						{
							Motor_X(0);
							printf("L_END");
							return;
						}		
					}					
				}
				else
				{
					//printf("LLF:LOCK=%.2f,Z=%.2f,TAG:%d \r\n",Z_Angle,Gyro[2],Z_Turn);	
					//mpu_dmp_get_data(&Gyro[2]);
					goto L1;
				}
			}
			else
			{
				Z_Turn = Z_Angle +85;
				while(1)
				{
					//printf("LF:LOCK=%.2f,Z=%.2f,TAG:%d \r\n",Z_Angle,Gyro[2],Z_Turn);
					if(Gyro[2] > Z_Turn )
					{
						Motor_X(0);
						printf("L_END");
						return;
					}					
				}			
			}	
	}
	Motor_X(0);
}
//***���ܺ�����

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  #ifdef W25Qxx
    SCB->VTOR = QSPI_BASE;
  #endif
  MPU_Config();
  CPU_CACHE_Enable();

	MotorOutput1 = 300;
	MotorOutput2 = 300;
	int i =0;
//	float Z_Angle = 0;
//	uint8_t Z_Turn = 0;
	uint8_t recv = 0x00;
	u8 Res_num = 0;
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
  MX_RTC_Init();
  MX_SPI4_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	LCD_Test();
  	__HAL_UART_CLEAR_IDLEFLAG(&huart2);
	
//	HAL_UART_Receive_IT(&huart2, (uint8_t *)RXbuf, 1); 

//	sprintf((char *)&text,"TI_Car");
//	LCD_ShowString(4, 6, 160, 14, 14, text);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	Motor_X(0);
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,350);//������
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,350);//������

	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_Base_Start_IT(&htim6);                // ʹ�ܶ�ʱ���ж�(5ms)
//	for(i = 0;i<5;i++)
//	{
//		Motor_X(i);
//		HAL_Delay(1000);
//	}
	
	//�������÷��ص�
	RESET:
	  /*
   *iic��ȡ����ID
   */
	  
	  HAL_I2C_Mem_Read(&hi2c1, (0x68 << 1), 0x75, I2C_MEMADD_SIZE_8BIT, &recv, 1, 0xfff);
	  if (recv == 0x68)
	  {
		printf("mpu6050 ID Read: OK at 0x68\r\n");
	  }
	  else
	  {
		printf("Err mpu id:0x%x\r\n", recv);
		goto RESET;
	  }


	  /*
	   *DMP��ʼ��
	   */
	  while (mpu_dmp_init())
	  {
		HAL_Delay(200);
	  }
	  HAL_Delay(500);
	  //�ؿ����񷵻ص�
	  REMAKE:
	  /*
	  �ȴ���������
	  */
		
	  /*
	  ��ʼ����
	  */	  
	  //Motor_X(3);
	  while (1)
	  {  
		  
		  //**Ѳ�ߵȵ�һ��ʮ��
			Res_num = Lock_Line();
	
	  }

	printf("END��\r\n");
	
	
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//HAL_GPIO_TogglePin(E3_GPIO_Port,E3_Pin);
    if(huart->Instance==USART2)
    {
        UART2_Rx_Buf[UART2_Rx_cnt] = UART2_temp[0];
        UART2_Rx_cnt++;
        if(0x0a == UART2_temp[0])
        {
            UART2_Rx_flg = 1;
        }

    }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    if (htim == (&htim6))
    {	//������� ֻ��ȡZ��Ƕ� 
		mpu_dmp_get_data(/*&Gyro[0],&Gyro[1],*/&Gyro[2]);  

		//printf("X=%.2f,Y=%.2f,Z=%.2f\r\n",Gyro[0],Gyro[1],Gyro[2]);
		//printf("Z=%.2f\r\n",Gyro[2]);
    }

}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  LED_Blink(500,500);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/