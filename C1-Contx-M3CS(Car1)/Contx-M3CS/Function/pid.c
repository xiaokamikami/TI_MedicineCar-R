#include "pid.h"
// 2.增量式PID控制器
int Error_Last1=0,Error_Prev1=0;          // 上一次偏差值，上上次误差
int Error_Last2=0,Error_Prev2=0;          // 上一次偏差值，上上次误差
int Pwm_add1 = 180,Pwm1 = 180,Pwm_last1 = 180;                    // PWM增量，PWM输出占空比
int Pwm_add2 = 180,Pwm2 = 180,Pwm_last2 = 180;                    // PWM增量，PWM输出占空比

float Kp = 0.1	, Ki = 0.1, Kd = 0.08;       // PID系数，这里只用到PI控制器
int SpeedInnerControl1(int Speed,int Target) // 速度内环控制
{
    int Error = Target - Speed ;		  // 偏差 = 目标速度 - 实际速度
//	if(-20.0 < Error < 20.0){
//		Pwm = Pwm_last;
//		return Pwm;
//	}
	if(Target == 0){
		Error_Last1=0,Error_Prev1=0;
	}	
    Pwm_add1 = Kp * (Error - Error_Last1) + 					  // 比例
			  Ki * Error +							    	  // 积分
			  Kd * (Error - 2.0f * Error_Last1 + Error_Prev1)	  // 微分
              ;  // 加一的目的是如果输出信号为0时，系统将进入失控状态
	
    Pwm1 = Pwm_add1+Pwm_last1;		              // 原始量+增量 = 输出量
	Pwm_last1 = Pwm1;					  //保存上次PWM值
	Error_Prev1 = Error_Last1;	  	  // 保存上上次误差
    Error_Last1 = Error;	              // 保存上次偏差

    if(Pwm1 >= 710) Pwm1 = 710;	      // 限幅
    if(Pwm1 <=-710) Pwm1 = -710;
	//Pwm1 = Pwm1 *-1;
    return Pwm1;	                      // 返回输出值
} 
int SpeedInnerControl2(int Speed,int Target) // 速度内环控制
{
    int Error = Target-Speed;		  // 偏差 = 目标速度 - 实际速度
//	if(-20.0 < Error < 20.0){
//		Pwm = Pwm_last;
//		return Pwm;
//	}
	if(Target == 0){
		Error_Last2=0,Error_Prev2=0;
	}	
    Pwm_add2 = Kp * (Error - Error_Last2) + 					  // 比例
			  Ki * Error +							    	  // 积分
			  Kd * (Error - 2.0f * Error_Last2 + Error_Prev2)	  // 微分
              ;  // 加一的目的是如果输出信号为0时，系统将进入失控状态
	
    Pwm2 = Pwm_add2+Pwm_last2;		              // 原始量+增量 = 输出量
	Pwm_last2 = Pwm2;					  //保存上次PWM值
	Error_Prev2 = Error_Last2;	  	  // 保存上上次误差
    Error_Last2 = Error;	              // 保存上次偏差

    if(Pwm2 >= 710) Pwm2 = 710;	      // 限幅
    if(Pwm2 <=-710) Pwm2 = -710;
	//Pwm2 = Pwm2 *-1;
    return Pwm2;	                      // 返回输出值
} 

