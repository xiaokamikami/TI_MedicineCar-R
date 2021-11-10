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
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "pid.h"
#include "control.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
static uint8_t Motor = 0;
static uint16_t Xunlu = 1;
uint8_t Mode_Set = 0 ;
static int Res_stick = 0;
static int time_slp = 100;
static uint8_t Mode = 0;
static uint8_t text[20];
static uint8_t Douck_V = 70 ; //舵机角度
static uint8_t Black_bock = 50;
static float Time_Sitck = 0;	//变换时间
static float Time_Sum = 0;		//累积时间
static uint8_t Time_Set = 10;	//时间设置
	static int Res = 0;

	static int pwm_ab = 0;
//	uint8_t Right_Flag = 0;
	static uint8_t Stop_Flag = 0;
	static uint8_t Star_Flag = 0;
	static uint8_t Black_Flag = 0;	//黑白计数
	static uint8_t Right_Flag = 0;	//黑白计数
extern int Pwm_add1 ,Pwm1 ,Pwm_last1 ;                    // PWM增量，PWM输出占空比
extern int Pwm_add2 ,Pwm2 ,Pwm_last2 ;                    // PWM增量，PWM输出占空比
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

static void Motor_Speed(int SpeedTarget1,int SpeedTarget2){
	  /* 调速 */	  	   
		if( Motor == 1 ){

				HAL_GPIO_TogglePin(E3_GPIO_Port,E3_Pin);

				//*********电机1**********//
				MotorOutput1 = SpeedInnerControl1(MotorSpeed1,SpeedTarget1);//PID控制器，取回占空比
				
				if(MotorOutput1 < 0){		//减速
//					HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_SET);	//电机1反转
//					HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_RESET);
//					__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,300);

//					HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_RESET);	//电机1正转
//					HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_SET);
					__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,0);
				}
				else{						//加速

//					HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_RESET);	//电机1正转
//					HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_SET);
					__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,MotorOutput1);

				}
				//*********电机2**********//
				MotorOutput2 = SpeedInnerControl2(MotorSpeed2,SpeedTarget2);//PID控制器，取回占空比
				
				if(MotorOutput2 < 0){		//减速
//					HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_SET);	//电机2反转
//					HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_RESET);
//					__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,300);
//					
//					HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_RESET);	//电机2正转
//					HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_SET);
					__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,0);
				}
				else{						//加速
//					HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_RESET);	//电机2正转
//					HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_SET);
					__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,MotorOutput2);

				}

	//			sprintf((char *)&aTxBuffer,"Mpd2=%d,Mpd1=%d,",MotorSpeed2,MotorSpeed1);
	//			HAL_UART_Transmit(&huart2, (uint8_t *)aTxBuffer, sizeof(aTxBuffer),10);

				Motor = 0;
			}
}
static void Xunlu_Bk(void){
	/**************** 巡线控制 ******************/	
			static uint8_t Res_num = 0;
		  /* 巡线-位置检测 */	
			if(	 Xunlu == 1 && Stop_Flag == 0){
				Res = 0;
				Res_num = 0;
//				SpeedTarget1 = 300;
//				SpeedTarget2 = 300;
//				if(Black_Flag == 0){
//					while(HAL_GPIO_ReadPin(R5_GPIO_Port,R5_Pin)==RESET  && HAL_GPIO_ReadPin(R3_GPIO_Port,R3_Pin)==RESET &&
//				    HAL_GPIO_ReadPin(R4_GPIO_Port,R4_Pin)==RESET && HAL_GPIO_ReadPin(R6_GPIO_Port,R6_Pin)==RESET    &&
//				    HAL_GPIO_ReadPin(R7_GPIO_Port,R7_Pin)==RESET && HAL_GPIO_ReadPin(R2_GPIO_Port,R2_Pin)==RESET	&&
//					HAL_GPIO_ReadPin(R1_GPIO_Port,R1_Pin)==RESET && HAL_GPIO_ReadPin(R8_GPIO_Port,R8_Pin)==RESET
//					);//等待非全白 进行调整		
					if(HAL_GPIO_ReadPin(R5_GPIO_Port,R5_Pin)==SET  || HAL_GPIO_ReadPin(R3_GPIO_Port,R3_Pin)==SET ||
				    HAL_GPIO_ReadPin(R4_GPIO_Port,R4_Pin)==SET || HAL_GPIO_ReadPin(R6_GPIO_Port,R6_Pin)==SET    ||
				    HAL_GPIO_ReadPin(R7_GPIO_Port,R7_Pin)==SET || HAL_GPIO_ReadPin(R2_GPIO_Port,R2_Pin)==SET	||
					HAL_GPIO_ReadPin(R1_GPIO_Port,R1_Pin)==SET || HAL_GPIO_ReadPin(R8_GPIO_Port,R8_Pin)==SET
					){	//如果有一个黑 进行调整
						HAL_GPIO_TogglePin(E3_GPIO_Port,E3_Pin);					
						if(HAL_GPIO_ReadPin(R1_GPIO_Port,R1_Pin)==SET)	{Douck_V =95;Res_num+=1;}		//右偏限位
						if(HAL_GPIO_ReadPin(R2_GPIO_Port,R2_Pin)==SET)	{Douck_V =90;Res_num+=1;}	
						if(HAL_GPIO_ReadPin(R3_GPIO_Port,R3_Pin)==SET)	{Douck_V =85;Res_num+=1;}
						if(HAL_GPIO_ReadPin(R4_GPIO_Port,R4_Pin)==SET)	{Douck_V =80;Res_num+=1;}	//右偏
						if(HAL_GPIO_ReadPin(R5_GPIO_Port,R5_Pin)==SET)	{Douck_V =70;Res_num+=1;}	//巡线中位	
						if(HAL_GPIO_ReadPin(R6_GPIO_Port,R6_Pin)==SET)	{Douck_V =60;Res_num+=1;}	//左偏	
						if(HAL_GPIO_ReadPin(R7_GPIO_Port,R7_Pin)==SET)	{Douck_V =57;Res_num+=1;}	
						if(HAL_GPIO_ReadPin(R8_GPIO_Port,R8_Pin)==SET)	{Douck_V =55;Res_num+=1;}		//左偏限
						Xunlu = 0;
						Black_Flag = 1;
						/* 时间计算 */	
						Black_bock --;
						Time_Sitck = Res_stick;
						Time_Sum += Res_stick;
						Res_stick = 0;

	//					if(Right_Flag ==0){


						/* 巡线-舵机位置控制 */	
							if(Res_num ==1){
								if(Black_bock <30){SpeedTarget1 = 90;SpeedTarget2 = 90;}
								if(Douck_V >80){Right_Flag =1;}
								else if(Right_Flag ==1){					//转向记录
									if(Douck_V <=90)	{Right_Flag = 2;}
									
								}
								if(Right_Flag ==2){							//完成转向
									SpeedTarget1 = 120;SpeedTarget2 = 120;
									Pwm_add1 = 200,Pwm1 =200,Pwm_last1 = 200;                    // PWM增量，PWM输出占空比
								    Pwm_add2 = 200,Pwm2 =200,Pwm_last2 = 200;                    // PWM增量，PWM输出占空比
									__HAL_TIM_SetCompare(&htim5,TIM_CHANNEL_3,70);
									Right_Flag++;
								}
								if(Right_Flag ==3){		
										/* 时间控制 */	
										if(Time_Sitck<(Time_Set/50) )
										{
											SpeedTarget1 = 90;SpeedTarget2 = 90;
										}
										else if( Time_Sitck > (Time_Set/50) )
										{
											SpeedTarget1 = 90;SpeedTarget2 = 90;
										}
								}
								else{
										if(Time_Sitck<(Time_Set/50) )
										{
											SpeedTarget1 = 90;SpeedTarget2 = 90;
										}
										else if( Time_Sitck > (Time_Set/50) )
										{
											SpeedTarget1 = 120;SpeedTarget2 = 120;
										}
								}
								//__HAL_TIM_SetCompare(&htim5,TIM_CHANNEL_3,Douck_V);
								
							}
							if(Res_num >=2){
								Douck_V = 70;
								__HAL_TIM_SetCompare(&htim5,TIM_CHANNEL_3,70);
								HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_SET);	//行车制动
								HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_RESET);
								HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_SET);
								HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_RESET);
								__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,800);
								__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,800);
								HAL_Delay(100);
								__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,0);
								__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,0);
								Stop_Flag = 1;
								Star_Flag = 0;
								HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);
								HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_2);
								HAL_TIM_PWM_Stop(&htim5,TIM_CHANNEL_3);
								HAL_TIM_Base_Stop_IT(&htim7);                // 关闭定时器中断(100ms)
								HAL_TIM_Base_Stop_IT(&htim6); 				// 关闭定时器中断(10ms)
								
							}							
//					}//等待非全白 进行调整					
					
					
					
		//		}
					
				
					
					
					//}
					
					
				}
//				else if(Black_Flag == 1){		
					if(HAL_GPIO_ReadPin(R5_GPIO_Port,R5_Pin)==RESET  && HAL_GPIO_ReadPin(R3_GPIO_Port,R3_Pin)==RESET &&
				    HAL_GPIO_ReadPin(R4_GPIO_Port,R4_Pin)==RESET && HAL_GPIO_ReadPin(R6_GPIO_Port,R6_Pin)==RESET    &&
				    HAL_GPIO_ReadPin(R7_GPIO_Port,R7_Pin)==RESET && HAL_GPIO_ReadPin(R2_GPIO_Port,R2_Pin)==RESET	&&
					HAL_GPIO_ReadPin(R1_GPIO_Port,R1_Pin)==RESET && HAL_GPIO_ReadPin(R8_GPIO_Port,R8_Pin)==RESET
					){
						Black_Flag =0;	
					}//等待非全白 进行调整	}
					Xunlu = 0;	
//				}
//				else if (Black_Flag == 1){
//					while(HAL_GPIO_ReadPin(R5_GPIO_Port,R5_Pin)==SET  || HAL_GPIO_ReadPin(R3_GPIO_Port,R3_Pin)==SET ||
//				    HAL_GPIO_ReadPin(R4_GPIO_Port,R4_Pin)==SET || HAL_GPIO_ReadPin(R6_GPIO_Port,R6_Pin)==SET    ||
//				    HAL_GPIO_ReadPin(R7_GPIO_Port,R7_Pin)==SET || HAL_GPIO_ReadPin(R2_GPIO_Port,R2_Pin)==SET	||
//					HAL_GPIO_ReadPin(R1_GPIO_Port,R1_Pin)==SET || HAL_GPIO_ReadPin(R8_GPIO_Port,R8_Pin)==SET
//					);//等待全白 进行调整
//					Black_Flag =0;						
//				}
					


			}
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
  /* 必须同时获取时间和日期 不然会导致下次RTC不能读取 */
  /* Both time and date must be obtained or RTC cannot be read next time */
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, sdatestructureget, RTC_FORMAT_BIN);
}

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

	SpeedTarget1 = 180;	  // 全局变量，LM速度目标值
	SpeedTarget2 = 180;	  // 全局变量，RM速度目标值
	MotorOutput1 = 300;
	MotorOutput2 = 300;
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
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_TIM7_Init();
  MX_TIM5_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	LCD_Test();

	
//	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,300-1);//调试用
//	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,300-1);//调试用
	HAL_GPIO_WritePin(Bee_GPIO_Port,Bee_Pin,GPIO_PIN_SET);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1); // 开启编码器A0
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_2); // 开启编码器A1
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1); // 开启编码器B0
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_2); // 开启编码器B1

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_RESET);	//电机方向
	HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_SET);
//	HAL_Delay(500);
	Star_Flag = 1;
  while (1)
  {   
	  
	if(Star_Flag == 1){
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_3);
//		int i = 0;
//		while(1){
//			i= 50;
//			for(i=60;i<100;i+=10){
				__HAL_TIM_SetCompare(&htim5,TIM_CHANNEL_3,70);
//				HAL_Delay(1000);
//				sprintf((char *)&text,"Duoji:%d  ",i);		
//				LCD_ShowString(4, 55, 160, 14, 14, text);
//			}
//			
//		}
		sprintf((char *)&text,"Steeing");		
		LCD_ShowString(4, 55, 160, 14, 14, text);
		HAL_Delay(5000);		//10秒的调试时间
		sprintf((char *)&text,"Open   ");		
		LCD_ShowString(4, 55, 160, 14, 14, text);
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,300);
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,300);
		//HAL_Delay(10);  //10ms启动电机时间
		HAL_TIM_Base_Start_IT(&htim7);                // 使能定时器中断(100ms)
		HAL_TIM_Base_Start_IT(&htim6);                // 使能定时器中断(10ms)
		HAL_GPIO_WritePin(Bee_GPIO_Port,Bee_Pin,GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(Bee_GPIO_Port,Bee_Pin,GPIO_PIN_SET);

	}


	while(Star_Flag == 1){ 
		 /* 时间切换  */
//		if(HAL_GPIO_ReadPin(KEY_GPIO_Port,KEY_Pin)== RESET){
//			while(HAL_GPIO_ReadPin(KEY_GPIO_Port,KEY_Pin)== RESET);
//			if(Time_Set<=20){
//				Time_Set ++;
//			}
//			else{Time_Set = 10;}
//			sprintf((char *)&text,"Time:%d          ",Time_Set);		
//			LCD_ShowString(4, 6, 160, 14, 14, text);
//			HAL_GPIO_TogglePin(E3_GPIO_Port,E3_Pin);
//		}
		SpeedTarget1 = 180;SpeedTarget2 = 180;
		  /* 模式切换 (角度) */
		if( Mode == 0){ }
				if(Motor ==0){     
			 // 1.获取电机速度
			GetMotorPulse();
			Motor = 1;
			// 3.将占空比导入至电机控制函数
			Motor_Speed(SpeedTarget1,SpeedTarget2);
		}

		sprintf((char *)&text,"Time:%d T_Sum:%.1lf  ",Time_Set,Time_Sum/10);		
		LCD_ShowString(4, 6, 160, 14, 14, text);	
		HAL_Delay(5);		
		sprintf((char *)&text,"POt1:%d,POt2:%d\r\n",MotorOutput1,MotorOutput2);		
		LCD_ShowString(4, 22, 160, 14, 14, text);
		HAL_Delay(5);
		sprintf((char *)&text,"Mpd1:%d,Mpd2:%d       ",MotorSpeed1,MotorSpeed2);		
		LCD_ShowString(4, 38, 160, 14, 14, text);
		HAL_Delay(5);
		while(Stop_Flag == 1){		//停止

		//LED_Blink(3,800);
		if(Stop_Flag == 1){
			HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_2);
			HAL_TIM_PWM_Stop(&htim5,TIM_CHANNEL_3);
			HAL_TIM_Base_Stop_IT(&htim7);                // 关闭定时器中断(100ms)
			HAL_TIM_Base_Stop_IT(&htim6); 				// 关闭定时器中断(10ms)
			HAL_GPIO_WritePin(Bee_GPIO_Port,Bee_Pin,GPIO_PIN_RESET);
			HAL_Delay(2000);
			HAL_GPIO_WritePin(Bee_GPIO_Port,Bee_Pin,GPIO_PIN_SET);
			Star_Flag = 0;
			
			Stop_Flag = 0;
		}
	}
	}
	
	
    
//		LED_Blink(3,500);
		
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_SPI4;
  PeriphClkInitStruct.Spi45ClockSelection = RCC_SPI45CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static unsigned char i = 0;
	static unsigned char dj = 70;
    if (htim == (&htim7))
    {	
			/* 舵机跟随目标值 */
			if(dj < Douck_V){
				if(( Douck_V - dj )<5){
					dj+=1;
				}
				else{dj+=5;
			}
				//dj = Douck_V;
			    __HAL_TIM_SetCompare(&htim5,TIM_CHANNEL_3,dj);
			}
			else if (dj > Douck_V){
				if(( dj - Douck_V )<5){
					dj-=1;
				}
				else{dj-=5;
				}
				
			    __HAL_TIM_SetCompare(&htim5,TIM_CHANNEL_3,dj);
			}
		Res_stick++;
		//if(i>2 && Xunlu >2 ){		  Xunlu = 1; }
		
       
		  //HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_2);
   		  //Xunlu++;

			//if(Xunlu ==0) {Xunlu = 1;}		
			
        }
    if (htim == (&htim6))
    {		
		Xunlu =1;	
		Xunlu_Bk();


    }
}
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */
	if(Time_Set<=20){
		Time_Set ++;
		}
	else{Time_Set = 10;}
	sprintf((char *)&text,"Time:%d T_Sum:%.1lf  ",Time_Set,Time_Sum/10);		
	LCD_ShowString(4, 6, 160, 14, 14, text);

	HAL_GPIO_TogglePin(E3_GPIO_Port,E3_Pin);
  /* USER CODE END EXTI15_10_IRQn 1 */
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
