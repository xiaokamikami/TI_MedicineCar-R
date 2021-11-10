//** stm
#include "main.h"

//** sys
#include "delay.h"
#include "lcd.h"
#include "key.h"
#include "pwm.h"
#include "Motor.h"
#include "bsp_nvic.h"
#include "encoder.h"
#include "TIM.h"
#include "gpio.h"

//**fun
#include "pid.h"


//** comm
//#include "DMA.h"
#include "usart.h"

//** USART
const u8 TEXT_TO_SEND[]={"ALIENTEK Mini STM32 DMA 串口实验"};
#define TEXT_LENTH  sizeof(TEXT_TO_SEND)-1			//TEXT_TO_SEND字符串长度(不包含结束符)
u8 SendBuff[(TEXT_LENTH+2)*100];

extern __IO int k2_num ;   //获取图像
extern __IO int k2_tag_x  ;   //目标横轴
//extern __IO int k2_val ;   //图像大小拟合距离 

//** Funthino
void Turn_Lift (u8 mode);		//90度左转
void Right_Lift (u8 mode);	    //90度右转
u8 Res_Read(u8 mode );		//读红外
void Photo_angle(uint16_t angle);//转头
void Color_LED(u8 led);	//点亮K210 RGB 0关 1红 2黄  3绿

static u16	L1_ENCODE = 0;//左编码器 += (abs(getTIMx_DetaCnt(TIM3)));
static u16  R1_ENCODE = 0;//右编码器 += (abs(getTIMx_DetaCnt(TIM5)));

static u8 Stock_tag  = 0;		//识别锁定数字

//** LCD
u32 TimingDelay = 0;
__IO u8 Key1_num = 0 ,Key3_num ,Key2_num, Res_near = 0;
int leftWheelEncoderNow    = 0;
int rightWheelEncoderNow   = 0;

//** USER

void Mian_Init(void);
void Dock_angle(uint16_t angle);	//180为最右  360最左

//** TAG
extern __IO u8 BALL_RX_FLAG ; //传输完成标志
u8 time_100ms = 0;
u8 time_200ms = 0;
u8 time_500ms = 0;
int Motor_Speed_Set  = 0;  //期望速度
__IO u8 sys_mode = 0;   //准备运行的题目
static u8 sys_runing = 0; //运行状态
//** Motor
uint16_t Douck_V = 270 ; //舵机期望角度
int Motor_Sp1 = 0;
int Motor_Sp2 = 0;
//extern int Pwm1,Pwm2;
static u16 dj = 270;			//舵机角度缓存  转弯时强制更新

static u8 back_cross = 0; //返回遇到的十字次数


	static u8 cross_num = 0;			//十字次数
	static u8 L_R = 0;   // 1左 2右  转向记录
	static u8 L_R_R = 0;   // 1左 2右  最大转向记录
	static u8 youtou = 1; 	//转头次数


//系统运行时间计数，，开机后开始计数，每秒增1
unsigned short  g_RunTime = 0;

void Delay_Ms(u32 nTime)
{
	u32 TIME2;
	TIME2=72;
	while(TimingDelay != 0){
		TIME2=72;
		while(TIME2!=0)
		{
			TIME2 --;
			
		}			
		TimingDelay --;
	}

	

}

//**页面切换			
void LCD_yemian(uint8_t mode,uint16_t Angle,uint8_t cross_num)
{	
	uint8_t * LCD_Buff[28];
	u8  huancun = 0;
	if(mode == 1)	//Setting
	{
		sprintf((char * )LCD_Buff,"White:TAG %d  ",Motor_Speed_Set );
		LCD_ShowString(0*8,3*16,(u8*)LCD_Buff,WHITE);
		Key2_num = 1;

		while(sys_runing == 1)		//设置病房
		{
				if(BALL_RX_FLAG == 1)	//检测到图像
				{
						if(Key2_num >5)		//确定目标	
						{						
						
							sys_runing = 2;
							Stock_tag = k2_num;
						
							break;
						}
						if(huancun !=k2_num )		
						{
							Key2_num  = 0;
							huancun = k2_num;
						}
						else{Key2_num+=1 ; }
						
						BALL_RX_FLAG = 0;
				}
				
				sprintf((char *)LCD_Buff,"House:%d",Stock_tag );
				LCD_ShowString(0*8,5*16,(u8*)LCD_Buff,WHITE);	
			delay_ms(10);
		}
		//决定模式
		if(Stock_tag <= 2)
		{
			sys_mode = 1;
		}
		else
		{
			sys_mode = 2;
		}
		
		sys_runing = 2;
		Key2_num = 1;
	}
	else if(mode == 2)	// ENCODE
	{
		sprintf((char * )LCD_Buff,"M1:%d,M2:%d ",L1_ENCODE,R1_ENCODE);
		LCD_ShowString(0*8,6*16,(u8*)LCD_Buff,RED);
	}
	else if(mode == 3)	//Back
	{
	
//		sprintf((char * )LCD_Buff,"X:%d,TAG:%d   ",k2_tag_x,Stock_tag);
//		LCD_ShowString(0*8,7*16,(u8*)LCD_Buff,RED);
		sprintf((char * )LCD_Buff,"M1:%d,M2:%d ",leftWheelEncoderNow,rightWheelEncoderNow);
		LCD_ShowString(0*8,8*16,(u8*)LCD_Buff,RED);
		sprintf((char * )LCD_Buff,"TAG:%d ",Motor_Speed_Set );
		LCD_ShowString(0*8,9*16,(u8*)LCD_Buff,RED);
		sprintf((char * )LCD_Buff,"T1:%d,T2:%d  ",Motor_Sp1,Motor_Sp2);
		LCD_ShowString(0*8,10*16,(u8*)LCD_Buff,RED);
	}
}	

	
//**Handle
void TIM2_IRQnHandler(void)   //TIM2中断  打开后记录电机转动圈数
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM1中断发生与否:TIM1 中断源 
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM1 中断源 
		L1_ENCODE += (abs(getTIMx_DetaCnt(TIM3)));
		R1_ENCODE += (abs(getTIMx_DetaCnt(TIM5)));
		LCD_yemian(2,1,1);
	}
	
}
void TIM1_UP_IRQHandler(void)   //TIM1中断
{
    static unsigned char i = 0;
	static unsigned char ip = 0;
	/* 这里写中断 */
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //检查指定的TIM1中断发生与否:TIM1 中断源 
	{
	   TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM1 中断源 	


//	   Get_Motor_Speed(&leftWheelEncoderNow,&rightWheelEncoderNow);  //得出编码器速度
////		if(ip == 2)
////		{

//				Motor_Sp1 = SpeedInnerControl1(leftWheelEncoderNow,Motor_Speed_Set );//PID控制电机1
//				if(Motor_Sp1 <= 0 )
//				{	
//					//MotorX_transfer(1);
//					TIM_SetCompare1(TIM8,0);;
//				}
//	
//				else if(Motor_Sp1 >0 )
//				{
//					//MotorX_transfer(2);
//					TIM_SetCompare1(TIM8,Motor_Sp1);
//				}
//				
//			
//				Motor_Sp2 = SpeedInnerControl2(rightWheelEncoderNow,Motor_Speed_Set );//PID控制电机2
//				if(Motor_Sp2 < 0 )
//				{	
//					//MotorX_transfer(1);
//					TIM_SetCompare2(TIM8,0);	
//				}
//				else if(Motor_Sp2 >0 )
//				{
//					TIM_SetCompare2(TIM8,Motor_Sp2);	
//					//MotorX_transfer(2);
//				}
//				
//		i +=1;
//		if(i==50){g_RunTime +=1;i=0;}
//		
	   
		R1_ENCODE += (abs(getTIMx_DetaCnt(TIM3)));
		L1_ENCODE += (abs(getTIMx_DetaCnt(TIM5)));

	}
}
void xunxian(u8 mode )
{
	u8 Res_numa;
	//拆解指令
	if(mode == 1)
	{

			LCD_yemian(3,1,1);
			Res_numa = Res_Read(1);						 
			if(Res_numa >=3)	//遇到第一个十字
			{
				if(Stock_tag == 1)
				{
					 Turn_Lift(1);		//左转
					 L_R = 1;
				}
				else if(Stock_tag == 2)
				{
					Right_Lift(1);		//右转
					L_R = 2;
				 }
				 sys_runing = 4;
				 cross_num = 1;
				 Res_numa = 0;
			
			}	
		//Res_num = 0;
	}
	else if(mode == 2)		//倒车巡线
	{
		//循环巡线
		while(Res_numa < 3)
		{
			Res_numa = Res_Read(2);	
			if(Res_numa >= 3)
			{
				MotorX_transfer(0);		//关电机					
			}
		}

	}

}

int main(void)
{

	static uint16_t Angle = 270;
	static uint8_t * LCD_buff[28];
	u8 i=0;
	Mian_Init();
	
	static u8 Res_num = 0;	//RES

	
	static u8  Lock_Phoht = 0; //锁定次数
	static u8 huancun = 0;
		
	
	
	while(1)
	{	
		LCD_Clear(BLACK);
		
		//************初始模式  		
		if(sys_runing == 1)
		{
			Key1_num =0;
			//**显示标题
			LCD_ShowString(10*8,0*16,(u8*)"STM-F103 ",YELLOW);
			LCD_ShowString(10*8,1*16,(u8*)"Setting    ",WHITE);
			while(BALL_RX_FLAG == 1 ){	}			//收到摄像头开机指令	
			LCD_yemian(1,1,1);
			while(sys_runing == 1)
			{
				LCD_yemian(1,Angle,1);
				
			}
			
			
			sys_runing = 2;
			
			BALL_RX_FLAG = 0;
		}
	
		//**等待指令
		else if(sys_runing == 2)
		{
			LCD_ShowString(10*8,0*16,(u8*)"STM-F103 ",YELLOW);
			LCD_ShowString(10*8,1*16,(u8*)"White Push    ",WHITE);
			sprintf((char *)LCD_buff,"Mode:%d",sys_mode );	//显示模式
			LCD_ShowString(0*8,7*16,(u8*)LCD_buff,RED);	
			sprintf((char *)LCD_buff,"House:%d",Stock_tag );	//显示房间号
			LCD_ShowString(0*8,2*16,(u8*)LCD_buff,RED);
			while(sys_runing == 2)		//等待指示
			{

				while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == 1){}	//等待物品到位
				sys_runing = 3;

			}
			BALL_RX_FLAG = 0;
			//跳出循环
		}
		

		//************出发模式
		else if(sys_runing == 3)
		{
	//**打开电机
			TIM8_PWM_Init(1000, 720); //1000 * 720 / 72MHz = 10MS = 1000HZ
			delay_ms(10);
			MotorX_Init(1);		//电机初始化
			MotorX_Init(2);	
			delay_ms(10);
			MotorX_transfer(1);//前进
			delay_ms(10);
			Timer1_Init(720,1000);  	//10ms
			//Timer2_Init(720,2000);  	//10ms
			
			//Timer1_Init(720,1000);  	//10ms
			//TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设 
			//TIM_Cmd(TIM1,DISABLE);  //关闭TIMx外设
			delay_ms(10);
		
			Motor_Speed_Set  = 256;		//目标速度设置
			Motor_Speed_Set  = 256;		//目标速度设置
			
			TIM_SetCompare1(TIM8,300);  //改变小车占空比
			TIM_SetCompare2(TIM8,300);	
//			for(i = 1;i<10;i++)
//			{
//				Motor_Speed_Set = 64*i;
//				LCD_yemian(3,1,1);
//				delay_ms(800);
//			}
	//**显示标题
			LCD_ShowString(5*8,0*16,(u8*)"STM-F103 ",RED);
			LCD_ShowString(5*8,1*16,(u8*)"Star..... ",WHITE);
			cross_num = 0;

			delay_ms(1000);
			TIM_SetCompare1(TIM8,180);  //改变小车占空比
			TIM_SetCompare2(TIM8,180);	
			Motor_Speed_Set  = 180;		//目标速度设置
			sprintf((char *)LCD_buff,"House:%d",Stock_tag );
			LCD_ShowString(0*8,3*16,(u8*)LCD_buff,WHITE);
			while(sys_runing == 3)
			{
//				sprintf((char * )LCD_buff,"dock:%d  ",Douck_V);
//				LCD_ShowString(0*8,5*16,(u8*)LCD_buff,WHITE);
				
					if(sys_mode == 1)
					{					
							LCD_ShowString(5*8,1*16,(u8*)"GO:1     ",WHITE);
							Motor_Speed_Set  = 180;		//目标速度设置
							MotorX_transfer(1);//前进
							while(sys_runing == 3){
								xunxian(1 );
//							sprintf((char * )LCD_buff,"dock:%d  ",Douck_V);
//							LCD_ShowString(0*8,5*16,(u8*)LCD_buff,WHITE);

							}					   
	
					}
					else if(sys_mode == 2)
					{
						LCD_ShowString(5*8,1*16,(u8*)"GO:2     ",WHITE);


						while(sys_runing == 3){

							Res_num = Res_Read(1);							 
							 if(Res_num >=3)	//遇到十字
							 {
								 cross_num +=1;
								 Dock_angle(270);
								if(cross_num >1)
								{
									MotorX_transfer(2);			//后退
									delay_ms(300);
									Motor_Speed_Set  = 0;
									Motor_Speed_Set  = 0;
									MotorX_transfer(0);			//关电机						
								}
																		
							 }
							 Res_num = 0;
						 
						//********等检测
						if(BALL_RX_FLAG == 1 && cross_num >1)			//第一个十字无视
						{						
	
							if(cross_num == 3 )			//第三次遇到直接进入判定 
							{
								if(Stock_tag == k2_num)
								{
									Lock_Phoht = 5;
								}
								
							}
							else if(Lock_Phoht <=5)		//确定目标	第二次
							{				
								if(huancun !=k2_num )		
								{
									Key2_num  = 0;
									huancun = k2_num;
									
								}
								else{Lock_Phoht+=1 ; }
							}
							//** 转弯判定	
							if(Lock_Phoht >5)
							{
								if(Stock_tag == k2_num && cross_num <= 2)	//第二十字
								{
									if(k2_tag_x < 80 ||youtou  == 0)		
									{
										Turn_Lift(1);		//左转
										L_R = 1;
									}
									else
									{
										Right_Lift(1);		//右转
										L_R = 2;
									}
									sys_runing = 4;	
									cross_num = 2;
									youtou = 0;								
								}
								else if(cross_num == 3 && Stock_tag == k2_num)	//第三个十字转向
								{										//需要等第四个
									if(youtou  == 0)		
									{
										Turn_Lift(1);		//左转
										L_R = 1;
									}
									else
									{
										Right_Lift(1);		//右转
										L_R = 2;
									}
									cross_num = 4;
									youtou = 0;	
									
								}
								else if(cross_num == 4 && Stock_tag)	//第四个十字转向
								{
									if(youtou  == 0)		
									{
										Turn_Lift(1);		//左转
										L_R_R = 1;
									}
									else
									{
										Right_Lift(1);		//右转
										L_R_R = 2;
									}
									youtou = 0;	
									sys_runing = 4;	
								}
								else if(Stock_tag != k2_num)//等第二次检测
								{
											
									if(youtou == 1)			//二次检测 确认数字
									{
										youtou += 1;
										Photo_angle(240);	//右转头
										delay_ms(300);

									}
									else if(youtou == 2)	//连续两次没认到 去下一个十字
									{
										youtou = 0;
										cross_num += 1;					 
										MotorX_transfer(1);			//前
										Photo_angle(260);	//右转头
										Motor_Speed_Set  = 128;
										delay_ms(500);
									}
								}
							}		
							//********检测完							
							BALL_RX_FLAG = 0 ;
									
							}
						}						
					}

				}
				
			BALL_RX_FLAG = 0;
			youtou = 0;
			TIM_SetCompare1(TIM8,100);  //改变小车占空比
			TIM_SetCompare2(TIM8,100);
			MotorX_transfer(1);	//				
			//跳出循环			
		}
		
		//************走向病房
		else if(sys_runing == 4)			
		{
			Dock_angle(270);
			LCD_ShowString(5*8,1*16,(u8*)"Run ..... ",WHITE);			
			TIM_SetCompare1(TIM8,120);  //改变小车占空比
			TIM_SetCompare2(TIM8,120);
			MotorX_transfer(1);	//
			while(sys_runing == 4)
			{
				//循环巡线
				Res_num = Res_Read(1);	
				//到位
				if(Res_num >= 3)
				{
					sys_runing = 5;
					MotorX_transfer(0);		//关电机					
				}	
			}
			
			BALL_RX_FLAG = 0;
			//跳出循环

		}
		//************出病房 回起点
		else if(sys_runing == 5)			
		{
			LCD_ShowString(5*8,1*16,(u8*)"Back ..... ",WHITE);
			LCD_ShowString(5*8,2*16,(u8*)"TAKE M ",WHITE);
			while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == 0){}	//等待物品拿出
			
			while(sys_runing == 5)
			{
				//需要倒车两次
				if(L_R_R != 0 )		
				{
					MotorX_transfer(1);			//回正
					
					xunxian(2 );//倒车巡线遇到十字跳出
					
					
					if(L_R_R == 2)
						{Right_Lift(2);}
					else if (L_R_R == 1)
						{Turn_Lift(2);}
						
				
					while(Res_num <= 1)			//第二十字
					{	//循环巡线
						 Res_num = Res_Read(1);	
						 Dock_angle(Douck_V);
					}	//到位
					
					
					if(L_R == 1)				//调头2
						{Right_Lift(2);}
					else if (L_R == 2)
						{Turn_Lift(2);}
							
					MotorX_transfer(1);			//前进
					Motor_Speed_Set  = 256;		//速度设置
					sys_runing = 6 ;
				}
				
				//需要倒车一次
				else if(L_R != 0)	
				{
					if(L_R == 2)
						{Right_Lift(2);}
					else if (L_R == 1)
						{Turn_Lift(2);}
							
					MotorX_transfer(1);			//前进
					
					sys_runing = 6 ;
				}
				TIM_SetCompare1(TIM8,0);  //改变小车占空比
				TIM_SetCompare2(TIM8,0);
				Dock_angle(270);			
				
			}
		}
		
		else if(sys_runing == 6)	
		{	
			LCD_ShowString(5*8,1*16,(u8*)"Backing ..... ",WHITE);
			
			MotorX_transfer(1);			//前进
			Motor_Speed_Set  = 256;		//速度设置
			while(sys_runing == 6)
			{
				if(cross_num == 1)
				{
					TIM_SetCompare1(TIM8,180);  //改变小车占空比
					TIM_SetCompare2(TIM8,180);
					while(Res_num <= 1)			//第1十字
					{	//循环巡线
						 Res_num = Res_Read(1);	
					     
					}	//到位
					if(Res_num >= 3)
					{
						Dock_angle(270);
					}
					TIM_SetCompare1(TIM8,0);  //改变小车占空比
					TIM_SetCompare2(TIM8,0);
					sys_runing = 7;
				}
				else if(cross_num == 2)
				{
					
					while(Res_num <= 1)			//第1十字
					{	//循环巡线
						 Res_num = Res_Read(1);	
						 Dock_angle(Douck_V);
					}	//到位
					
					
					if(Res_num >= 3)
					{
						Dock_angle(270);
						back_cross += 1;
						Res_num = 0;
					}			
					delay_ms(200);
					Motor_Speed_Set  = 180;		//速度设置
					while(Res_num <= 1)			//第二十字
					{	//循环巡线
						 Res_num = Res_Read(1);	
						 Dock_angle(Douck_V);
					}	//到位
					Dock_angle(270);
					
					if(Res_num >= 3)
					{
						Dock_angle(270);
						back_cross += 1;
						Res_num = 0;
					}	
				}	
				else if(cross_num == 4)
				{
					
				}	
			}
		}
		
	}
}

//读取对管数据
u8 Res_Read(u8 mode )		//1前进模式  2倒车模式
{
	u8 Res_n = 0;
	Res_n = 0;
	if(mode == 1)
	{
//	if(GPIO_ReadInputDataBit(GPIOD,RES1)==0  || GPIO_ReadInputDataBit(GPIOB,RES2)==0 ||
//		GPIO_ReadInputDataBit(GPIOB,RES3)==0 || GPIO_ReadInputDataBit(GPIOB,RES4)==0    ||
//		 GPIO_ReadInputDataBit(GPIOB,RES5)==0 || GPIO_ReadInputDataBit(GPIOB,RES6)==0	)
//	{
		if(GPIO_ReadInputDataBit(GPIOD,RES1)==1)	{Douck_V =360;Res_n+=1;}		//左偏限位
		if(GPIO_ReadInputDataBit(GPIOB,RES2)==1)	{Douck_V =330;Res_n+=1;}	//左偏
		if(GPIO_ReadInputDataBit(GPIOB,RES3)==1)	{Douck_V =300;Res_n+=1;}	//巡线中位
		if(GPIO_ReadInputDataBit(GPIOA,RES4)==1)	{Douck_V =270;Res_n+=1;}	//巡线中位
		if(GPIO_ReadInputDataBit(GPIOB,RES5)==1)	{Douck_V =230;Res_n+=1;}	//右偏	
		if(GPIO_ReadInputDataBit(GPIOB,RES6)==1)	{Douck_V =200;Res_n+=1;}		//右偏限
		

//	}
	}
	else if(mode == 2)
	{
		if(GPIO_ReadInputDataBit(GPIOD,RES1)==1)	{Douck_V =200;Res_n+=1;}		//左偏限位
		if(GPIO_ReadInputDataBit(GPIOB,RES2)==1)	{Douck_V =230;Res_n+=1;}	//左偏
		if(GPIO_ReadInputDataBit(GPIOB,RES3)==1)	{Douck_V =270;Res_n+=1;}	//巡线中位
		if(GPIO_ReadInputDataBit(GPIOA,RES4)==1)	{Douck_V =300;Res_n+=1;}	//巡线中位
		if(GPIO_ReadInputDataBit(GPIOB,RES5)==1)	{Douck_V =330;Res_n+=1;}	//右偏	
		if(GPIO_ReadInputDataBit(GPIOB,RES6)==1)	{Douck_V =360;Res_n+=1;}		//右偏限	
	}
	if(Res_n == 0)
	{return Res_n;}
	else if(Res_n == 1)
	{
		Dock_angle(Douck_V);
	}
	else if(Res_n > 1)	
	{
		Dock_angle(270);
		//Douck_V = 270;
	}
	return Res_n;
}

//90度右转			1正 2倒
void Right_Lift (u8 mode) 			
{
	u16  L_TAG,R_TAG;
	u16  Back_TAG;


	MotorX_transfer(0);	//STOP
	L1_ENCODE += (abs(getTIMx_DetaCnt(TIM3)));
	R1_ENCODE += (abs(getTIMx_DetaCnt(TIM5)));
	L1_ENCODE = 0,R1_ENCODE = 0;
	//重新开始编码器计数		
	TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设 
	
		u8 Res_numb = 0;
		if(mode == 2)			//车头朝右的情况倒出
		{
			L_TAG = 3300;
			R_TAG = 3300;
			Back_TAG = 2600;
			Dock_angle(270);
			TIM_SetCompare1(TIM8,350);  //改变小车占空比
			TIM_SetCompare2(TIM8,350);
			MotorX_transfer(2);	//倒车
			while(1)
			{
				LCD_yemian(2,1,1);
				if(L1_ENCODE <= Back_TAG)
				{
					Motor_L (2);			
				}
				else 
				{
					MotorX_Break(1);	
				}
				if(R1_ENCODE <= Back_TAG)
				{
					Motor_L (2);			
				}
				else 
				{
					MotorX_Break(1);	
				}
				if(R1_ENCODE >Back_TAG && L1_ENCODE > Back_TAG)
				{break;}				
			}
			L1_ENCODE = 0,R1_ENCODE = 0;
			MotorX_transfer(0);	//倒车
			Dock_angle(180);
			while(1)
			{

				LCD_yemian(2,1,1);

				if(L1_ENCODE <= L_TAG)
				{
					Motor_L (1);				
				}
				else if(L1_ENCODE > L_TAG)
				{
					MotorX_Break(1);		
				}
				if(R1_ENCODE <=R_TAG)
				{
					Motor_R (2);
				}
				else if(R1_ENCODE >R_TAG)
				{
					MotorX_Break(2);
				}
				if(R1_ENCODE >R_TAG && L1_ENCODE > L_TAG)
				{break;}
				
			}
		}
			//*********************** 1
		else if(mode ==1)
		{	
			
			L_TAG = 4500;
			R_TAG = 3000;
			Back_TAG = 500;
			Dock_angle(270);
			TIM_SetCompare1(TIM8,350);  //改变小车占空比
			TIM_SetCompare2(TIM8,350);
			MotorX_transfer(2);	//倒车
			while(1)
			{
				LCD_yemian(2,1,1);
				if(L1_ENCODE <= Back_TAG)
				{
					Motor_L (2);			
				}
				else 
				{
					MotorX_Break(1);	
				}
				if(R1_ENCODE <= Back_TAG)
				{
					Motor_L (2);			
				}
				else 
				{
					MotorX_Break(1);	
				}	
				if(R1_ENCODE >Back_TAG && L1_ENCODE > Back_TAG)
				{break;}					
			}
			L1_ENCODE = 0,R1_ENCODE = 0;
			MotorX_transfer(0);	//倒车
			Dock_angle(360);
			while(1)
			{
				LCD_yemian(2,1,1);

				if(L1_ENCODE <= L_TAG)
				{
					Motor_L (1);				
				}
				else if(L1_ENCODE > L_TAG)
				{
					MotorX_Break(1);		
				}
				if(R1_ENCODE <=R_TAG)
				{
					Motor_R (2);
				}
				else if(R1_ENCODE >R_TAG)
				{
					MotorX_Break(2);
				}
				if(R1_ENCODE >R_TAG && L1_ENCODE > L_TAG)
				{break;}
				
			}
		}
		//**完成转弯 退回巡线
	TIM_Cmd(TIM1, DISABLE);  //关闭TIMx外设 
}

//90度左转			
void Turn_Lift(u8 mode)		
{
	if(mode == 2)
	{
		
	}
	else if(mode ==1)
	{
		

	}
		
}

//前车架转向 180为最右  360最左
void Dock_angle(uint16_t angle)	
{
	uint16_t Docker;
	Docker = (int)(angle/18);
	if(Docker <10)	   {Docker = 10;}
	else if(Docker >20){Docker = 20;}
	TIM_SetCompare2(TIM4,Docker);
	
	delay_ms(50);
	//TIM_SetCompare2(TIM4,0);
}

//摄像头转头 180为最右  360最左  
void Photo_angle(uint16_t angle)	
{
	uint16_t Docker;
	Docker = (int)(angle/18);
	if(Docker <12)	   {Docker = 12;}
	else if(Docker >18){Docker = 18;}
	TIM_SetCompare1(TIM4,Docker);
	
	//delay_ms(350);
	//TIM_SetCompare2(TIM4,0);
}



void Mian_Init(void)
{
	SysTick_Config(SystemCoreClock/1000);
	delay_init();
	//delay_ms(500);
	
	//**外设初始化
	KEY_Init();
	RES_Init();	//对管初始化
	Lcd_Init();
	//DMA1通道4,外设为串口1,存储器为SendBuff,长(TEXT_LENTH+2)*100.
	LCD_Clear(BLACK);
	BACK_COLOR = BLACK;
	//LCD_ShowString(6*8,0*16,(u8*)"PUSH KEY1 Star",WHITE);
	
	/* 初始化USART */
	uart_init(115200);	
	
	//while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13) == 1){} //按键开始自检
	delay_ms(100);
		
	//**打开舵机
	TIM4_PWM_Init(200,7200,20);	//20MS = 50HZ  PLS 0.5MS-2.5MS	

	//**执行机械部分自检
	
		//**摄像头自检	正式部分不用
//		Photo_angle(360);
//		delay_ms(1000);
//		Photo_angle(180);	//右转
//		delay_ms(1000);
		Photo_angle(260);	//左转
		delay_ms(1000);
		
		//**转向自检
		//Dock_angle(180);
		Dock_angle(270);
		
//			//**指示灯自检
//			Color_LED(3);
//			delay_ms(500);
//			Color_LED(2);
//			delay_ms(500);
//			Color_LED(1);
//			delay_ms(500);	
//			Color_LED(0);

	//***********************************************
	//**编码器初始化
	Encoder_Init_TIM3();
	Encoder_Init_TIM5();

	//EXTI_Key_Config();		按键中断
	sys_runing =1;
//	LCD_ShowString(28*8,14*16,(u8*)"1",WHITE);
}

void SysTick_Handler(void)			//IT.c中的滴答定时器
{
	TimingDelay--;
}
