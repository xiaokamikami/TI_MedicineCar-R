#include "pid.h"
// 2.增量式PID控制器
int Error_Last1=0,Error_Prev1=0;          // 上一次偏差值，上上次误差
int Error_Last2=0,Error_Prev2=0;          // 上一次偏差值，上上次误差
int Pwm_add1 = 500,Pwm1 =500,Pwm_last1 = 500;                    // PWM增量，PWM输出占空比
int Pwm_add2 = 500,Pwm2 =500,Pwm_last2 = 500;                    // PWM增量，PWM输出占空比
float Kp = 0.3	, Ki = 0.1, Kd = 0.08;       // PID系数，这里只用到PI控制器

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

    if(Pwm1 >= 1500) Pwm1 = 1500;	      // 限幅
    if(Pwm1 <=-1500) Pwm1 = -1500;
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

    if(Pwm2 >= 1500) Pwm2 = 1500;	      // 限幅
    if(Pwm2 <=-1500) Pwm2 = -1500;
    return Pwm2;	                      // 返回输出值
} 

//pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
typedef struct PID
{
float kp;
float ki;
float kd;
float ek;     //当前误差
float ek_1;   //上一次误差
float ek_sum; //误差总和
float limit;  //限幅
}PID; 
static PID pid;
void PID_Init()
{
    pid.kp = 1;
    pid.ki = 0;
    pid.kd = 0;
    pid.limit = 0;
    pid.ek = 0;
    pid.ek_1 = 0;
    pid.ek_sum = 0;
}
// 位置式PID控制
float PID_Postion(int Infrared,int Target)
{
	int pwm = 0;
		pid.ek = Target - Infrared; // 计算当前误差
		pid.ek_sum += pid.ek;      //求出偏差的积分
		pwm = pid.kp*pid.ek + pid.ki*pid.ek_sum + 
		pid.kd*(pid.ek - pid.ek_1);   //位置式PID控制器
		pid.ek_1 = pid.ek;   //保存上一次偏差 
	if(pid.limit >= 1000) pid.limit = 1000;	      // 限幅
    else if(pid.limit <=-1000) pid.limit = -1000;
	if(pwm > pid.limit)
		{
		  pwm =  pid.limit;
		}
	else if(pwm < -pid.limit)
		{
		  pwm =  -pid.limit;
		}
	
return pwm;
}