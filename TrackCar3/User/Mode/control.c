#include "control.h"
#include "math.h"
#include "tim.h"
#include "stm32f1xx_it.h"
#include "OpenmvData.h"
#include "gpio.h"

unsigned int BMotorPulse,AMotorPulse;//全局变量,保存B电机脉冲数值
void CarTracking(void);
void CarBackTracking(void);

void CarBackDistance(uint16_t tag);
int SpeedTarget = 0;//全局变量,电机速度目标值

int ErrorPrev,PulsePrev;//上一次偏差值，上一次速度
int PwmBais,Pwm;//PWM增量， PWM总量
int Error,Pulse;//偏差
extern __IO uint32_t MotorCouter1,MotorCouter2;

extern unsigned int Idata;
void CarBackStraight(void);
void CarStop(void);
void Motor_dict(uint8_t Motox,uint8_t mode);

void Motor_dict(uint8_t Motox,uint8_t mode)
{
    if(Motox == 1)
    {
        if(mode == 1)
        {
            HAL_GPIO_WritePin(EN1_GPIO_Port,EN1_Pin,GPIO_PIN_RESET);	//电机1正转
            HAL_GPIO_WritePin(EN2_GPIO_Port,EN2_Pin,GPIO_PIN_SET);
        }
        else if(mode ==2)
        {
            HAL_GPIO_WritePin(EN1_GPIO_Port,EN1_Pin,GPIO_PIN_SET);	//电机2反转
            HAL_GPIO_WritePin(EN2_GPIO_Port,EN2_Pin,GPIO_PIN_RESET);
        }
    }
    else if(Motox == 2)
    {
        if(mode == 1)
        {
			HAL_GPIO_WritePin(EN3_GPIO_Port,EN3_Pin,GPIO_PIN_RESET);	//电机2正转
			HAL_GPIO_WritePin(EN4_GPIO_Port,EN4_Pin,GPIO_PIN_SET);
        }
        else if(mode ==2)
        {
            HAL_GPIO_WritePin(EN3_GPIO_Port,EN3_Pin,GPIO_PIN_SET);	//电机2反转
            HAL_GPIO_WritePin(EN4_GPIO_Port,EN4_Pin,GPIO_PIN_RESET);
        }
    }

}

void GetMotorPulse(void)//读取电机B,A的脉冲
{
	
	BMotorPulse = (short)(__HAL_TIM_GET_COUNTER(&htim4));//获取B电机计数器值
	__HAL_TIM_SET_COUNTER(&htim4,0);//B电机计数器清零
	
	AMotorPulse = (short)(__HAL_TIM_GET_COUNTER(&htim2));//获取A电机计数器值
	__HAL_TIM_SET_COUNTER(&htim2,0);//A电机计数器清零
	
}

int SpeedInnerControl1(int BPulse,int Target)//B电机速度内环控制：BPulse实际速度，BTarget目标速度
{


	float fP = 3, fI = 0.1;//这里只用到PI， 参数由电机的种类和负载决定
	
	Pulse = (BPulse + BPulse) / 2;//求A,B两电机速度的平均值
	Pulse = 0.7 * PulsePrev + 0.3 * Pulse;//低通滤波，使速度更平滑
	PulsePrev = Pulse;//保存前一次速度
	
	Error = Target - Pulse;//偏差 = 目标速度 - 实际速度
	PwmBais = fP * (Error - ErrorPrev) + fI * Error;//增量式PI控制器
	ErrorPrev = Error;//保存上一次偏差
	Pwm += PwmBais;//增量输出
	
	if(Pwm > 2200) Pwm = 2200;//限制上限， 防止超出PWM量程
	if(Pwm < 1000) Pwm = 1000;
	
	/******模拟示波器输出量设置**********/
//	OutData[0]=(float)BPulse;//速度实际值[红线]
//	OutData[3]=(float)APulse;//速度实际值[粉线]
//	OutData[1]=(float)Target ;//速度目标值[黄线]
//	OutData[2]=(float)Pwm;//PWM输出值[蓝线]
	
	return Pwm;//返回B电机输出值
	
}
int SpeedInnerControl2(int APulse,int Target)//B电机速度内环控制：BPulse实际速度，BTarget目标速度
{


    float fP = 3, fI = 0.1;//这里只用到PI， 参数由电机的种类和负载决定

    Pulse = (APulse + APulse) / 2;//求A,B两电机速度的平均值
    Pulse = 0.7 * PulsePrev + 0.3 * Pulse;//低通滤波，使速度更平滑
    PulsePrev = Pulse;//保存前一次速度

    Error = Target - Pulse;//偏差 = 目标速度 - 实际速度
    PwmBais = fP * (Error - ErrorPrev) + fI * Error;//增量式PI控制器
    ErrorPrev = Error;//保存上一次偏差
    Pwm += PwmBais;//增量输出

    if(Pwm > 2200) Pwm = 2200;//限制上限， 防止超出PWM量程
    if(Pwm < 1000) Pwm = 1000;

    /******模拟示波器输出量设置**********/
//	OutData[0]=(float)BPulse;//速度实际值[红线]
//	OutData[3]=(float)APulse;//速度实际值[粉线]
//	OutData[1]=(float)Target ;//速度目标值[黄线]
//	OutData[2]=(float)Pwm;//PWM输出值[蓝线]

    return Pwm;//返回B电机输出值

}
void VoltageAndDirection(int Pwm1,int Pwm2)//设置电机B,A电压和方向             前进
{
	
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1,Pwm1);
         Motor_dict(1,1);

		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,Pwm2);
         Motor_dict(2,1);
	
}

void OpposeVoltageAndDirection(int Pwm1,int Pwm2)//设置电机B,A电压和方向        后退
{

        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1,Pwm1);
         Motor_dict(1,2);

        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,Pwm1);
        Motor_dict(2,2);

}

void LeftDifferential(int Pwm1,int Pwm2)        //左差数
{
            __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1,Pwm1);
    Motor_dict(2,2);

    Motor_dict(1,1);
            __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,Pwm2);//B电机
}

void RightDifferential(int Pwm1,int Pwm2)        //右差数
{
            __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1,Pwm1);//A电机
    Motor_dict(1,2);

    Motor_dict(2,1);
            __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,Pwm2);



}
void Res_Rback(void)        //使小车回到线上
{
    do                    //检测
    {
        state = InfraredScan();//红外扫描,获取角度值

    } while (state <= 0);
}
/*
*********************************************************************************************************
*	函 数 名: Camera
*	功能说明: 摄像头检测是否在这个路口 如果是 在 左（1） 右（0）
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
int Camera(void)
{
    //舵机转动 左 检测数字是否 在左侧
    //舵机

    //摄像头检测
    while(!(OpenmvData()));

    //判断是否是 相关的数字
    if(Xdate == TempXdate)
    {
        return 1;
    }

    // 舵机转动 右 检测数字是否 在右侧
    //舵机

    //摄像头检测
    while(!(OpenmvData()));

    //判断是否是 相关的数字
    if(Xdate == TempXdate)
    {
        return 0;
    } else
    {
        return 2; //不在这个路口
    }


}


/*
*********************************************************************************************************
*	函 数 名: CameraTwo
*	功能说明: 第二个路口 4个图片 4个位置
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
int CameraTwo(void)
{
    //舵机转动 左2 检测数字是否 在左侧
    //舵机

    //摄像头检测
    while(!(OpenmvData()));

    //判断是否是 相关的数字
    if(Xdate == TempXdate)
    {
        return 1;
    }

    //舵机转动 左1 检测数字是否 在左侧
    //舵机

    //摄像头检测
    while(!(OpenmvData()));

    //判断是否是 相关的数字
    if(Xdate == TempXdate)
    {
        return 1;
    }

    // 舵机转动 右1 检测数字是否 在右侧
    //舵机

    //摄像头检测
    while(!(OpenmvData()));

    //判断是否是 相关的数字
    if(Xdate == TempXdate)
    {
        return 0;
    }

    // 舵机转动 右2 检测数字是否 在右侧
    //舵机

    //摄像头检测
    while(!(OpenmvData()));

    //判断是否是 相关的数字
    if(Xdate == TempXdate)
    {
        return 0;
    }


}



/*
*********************************************************************************************************
*	函 数 名: CarStart
*	功能说明: 开始
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarStart(void)
{
    CarStop();
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 75);//输出小车角度值 归正
    SpeedTarget = 200;                              //速度设置（巡线要快 抢时间）
    MovingDirection = 1;                            //前进
    HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断   开启电机
}


/*
*********************************************************************************************************
*	函 数 名: CarStop
*	功能说明: 停车
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarStop(void)
{
    HAL_TIM_Base_Stop_IT(&htim6);   //  关闭定时器更新中断    关闭电机
    VoltageAndDirection(0,0);
    Pwm=0;                          //  电机停止
    ErrorPrev=0;
    PulsePrev=0;
    PwmBais=0;
    Error=0;
    Pulse=0;
    BMotorPulse = 0;
    AMotorPulse = 0;
    MotorCouter1 = 0,MotorCouter2 = 0;
    __HAL_TIM_SET_COUNTER(&htim2,0);//A电机计数器清零
    __HAL_TIM_SET_COUNTER(&htim4,0);//B电机计数器清零


    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1,0);
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,0);//A电机

    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3,0);
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4,0);//B电机

}


/*
*********************************************************************************************************
*	函 数 名: CarOver
*	功能说明: 检测有没有门口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarOver(void)
{

        do                                              //检测有没有门口
        {
            state = InfraredScan();//红外扫描,获取角度值
        } while (!(state < 0));

}


/*
*********************************************************************************************************
*	函 数 名: CarBackDistance
*	功能说明: 小车后退一定距离
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarBackDistance(uint16_t tag)
{   //差速强转
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 70);//输出小车角度值 回归平行
    SpeedTarget = 300;                              //速度设置（转弯 速度慢） -400~-300

    MotorCouter1=0,MotorCouter2= 0;
    while ( 1)
    {

        MovingDirection = 2;
        if ( MotorCouter1 >tag && MotorCouter2 > tag)
        {break;}
        //printf("%d\r\n",MotorCouter);
        state = InfraredScan();//红外扫描,获取角度值
        switch (state) {
            /****维持向前行驶****/
            case 4 :
                angle = 75;
                break;//左偏，向右修正
            case 8 :
                angle = 65;
                break;//右偏，向左修正
            case 16:
                angle = 85;
                break;
            case 2:
                angle = 55;
                break;
        }
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, angle);//输出小车角度值
    }
    MovingDirection = 1;                            //后退
    //第三步 停车
    CarStop();

}


/*
*********************************************************************************************************
*	函 数 名: CarBackStraight
*	功能说明: 小车直线后退
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarBackStraight(void)
{
    SpeedTarget = -500;                              //速度设置（巡线要快 抢时间）
    MovingDirection = 2;                            //后退
    HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断   开启电机
}


/*
*********************************************************************************************************
*	函 数 名: CarTracking
*	功能说明: 初步的巡线
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarTracking(void)
{
    do                                              //初步的巡线
    {
        state = InfraredScan();//红外扫描,获取角度值
        switch (state) {

            /****维持向前行驶****/
            case 8 :
                angle = 65;
                break;//左灯灭右偏，向左修正，角度越小左转向
            case 4 :
                angle = 75;
                break;//右灯灭左偏，向右修正，角度越大右转向
                // /****维持****/
            case 16 :
                angle = 55;
                break;//左灯灭右偏，向左修正，角度越小左转向
            case 2 :
                angle = 85;
                break;//右灯灭左偏，向右修正，角度越大右转向
            case 1 :
                angle = 100;
                break;//右灯灭左偏，向右修正，角度越大右转向
            case 32 :
                angle = 50;
                break;//右灯灭左偏，向右修正，角度越大右转向
        }
                __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, angle);//输出小车角度值
    } while (state != 0x0f);
    //MovingDirection = 2;                            //倒一点车
    //HAL_Delay(400);
}


/*
*********************************************************************************************************
*	函 数 名: CarBackTracking
*	功能说明: 后退的巡线
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarBackTracking(void)
{
    //第三步 巡线 检测病房入口             （未加入口后防抖动）！！！！！！！
    do                                              //初步的巡线
    {
        state = InfraredScan();//红外扫描,获取角度值
        switch (state) {

            /****维持向前行驶****/
            case 1 :
                angle = 50;
                break;//左偏，向右修正
            case 2:
                angle = 60;
                break;//右偏，向左修正
            case 4:
                angle = 70;
                break;
            case 8:
                angle = 80;
                break;
            case 16 :
                angle = 90;
                break;//右灯灭左偏，向右修正，角度越大右转向
            case 32 :
                angle = 100;
                break;//右灯灭左偏，向右修正，角度越大右转向

        }
                __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, angle);//输出小车角度值
    } while (state != 0x0f);                                                //判断是否检测到一根红线
}

/*
*********************************************************************************************************
*	函 数 名: CarCallTracking
*	功能说明: 小车返回的巡线 寻找入口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarCallTracking(void)
{
    do                                              //初步的巡线
    {
        state = InfraredScan();//红外扫描,获取角度值
        switch (state) {
            /****维持向前行驶****/
            case 8 :
                angle = 65;
                break;//左灯灭右偏，向左修正，角度越小左转向
            case 4 :
                angle = 75;
                break;//右灯灭左偏，向右修正，角度越大右转向
                // /****维持****/
            case 16 :
                angle = 55;
                break;//左灯灭右偏，向左修正，角度越小左转向
            case 2 :
                angle = 85;
                break;//右灯灭左偏，向右修正，角度越大右转向
            case 1 :
                angle = 100;
                break;//右灯灭左偏，向右修正，角度越大右转向
            case 32 :
                angle = 50;
                break;//右灯灭左偏，向右修正，角度越大右转向
        }
                __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, angle);//输出小车角度值
    } while (Idata <2);
}


/*
*********************************************************************************************************
*	函 数 名: CarLeftTurn
*	功能说明: 小车左转
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarLeftTurn(void)      //小车左转  （后退加差数）
{
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 65);//向右转
    HAL_Delay(100);
    SpeedTarget = 120;                              //速度设置（转弯 速度慢）
    MovingDirection = 4;                            //左差数
    HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断   开启电机
    MotorCouter1 = 0,MotorCouter2 = 0;

    while (1 )
    {
        MovingDirection = 4;                            //左差数
        printf("%d\r\n",MotorCouter2);
        if(MotorCouter1 >3600 && MotorCouter2 > 3600)
        {break;}
    }

    MovingDirection = 1;
}

/*
*********************************************************************************************************
*	函 数 名: CarRightTurn
*	功能说明: 小车右转
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarRightTurn(void)      //小车右转  （后退加差数）
{

    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 100);//向右转
    HAL_Delay(100);
    SpeedTarget = 120;                              //速度设置（转弯 速度慢）
    MovingDirection = 3;                            //左差数
    HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断   开启电机
    MotorCouter1 = 0,MotorCouter2 = 0;

    while (1 )
    {
        MovingDirection = 3;                            //左差数
        printf("%d\r\n",MotorCouter1);
        if(MotorCouter1 >3600 && MotorCouter2 > 3600)
        {break;}
    }

    MovingDirection = 1;

}


/*
*********************************************************************************************************
*	函 数 名: BaseOne
*	功能说明: 数字任务一
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void BaseOne(void)          //数字一
{
    //检测药物
    while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == SET);

    //第一步   起步
    CarStart();

    //巡线
    CarTracking();

    //后退
    CarBackDistance(700);

    //第二步 左转弯  带追平
    CarLeftTurn();

    //第三步 巡线 检测病房入口
    Res_Rback();    //等待回归线上
    HAL_Delay(100);
    //巡线
    CarCallTracking();
   // CarTracking();
   // CarCallTracking();
  //  CarOver();



    CarStop();
    //点亮红灯
    HAL_GPIO_WritePin(Red_GPIO_Port,Red_Pin,RESET);

    //等待卸货
    while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

    //熄灭红灯
    HAL_GPIO_WritePin(Red_GPIO_Port,Red_Pin,SET);

    //原路返回
    //后退
    CarBackStraight();

    //等待一根红线 （后退相反检测）   未写完
    CarBackTracking();

    //后退一段距离
    CarBackDistance(350);

    //第二步 左转弯
    CarLeftTurn();

    //起步
    CarStart();
    Res_Rback();    //等待回归线上
    HAL_Delay(100);
    //巡线
    CarCallTracking();
 //   CarTracking();
 //   CarOver();
    //停
    CarStop();

    //点亮绿灯
    HAL_GPIO_WritePin(Green_GPIO_Port,Green_Pin,RESET);
    while(1);
}

/*
*********************************************************************************************************
*	函 数 名: BaseTwo
*	功能说明: 数字任务二
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void BaseTwo(void)        //数字二
{
    //检测药物
    while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == SET);

    //第一步   起步
    CarStart();
    printf("STAR_OK");
    //巡线
    CarTracking();
    printf("XUN_OK");
    //后退
    CarBackDistance(700);

    //第二步 右转弯  带追平
    CarRightTurn();
    printf("RF_OK");
    //第三步 巡线 检测病房入口
    //巡线
    //CarOver();
    Res_Rback();    //等待回归线上
    HAL_Delay(100);
    //巡线
    printf("ROOM_");
    //巡线
    CarCallTracking();
    //CarTracking();
   // CarOver();
    CarStop();
    //点亮红灯
    HAL_GPIO_WritePin(Red_GPIO_Port,Red_Pin,RESET);

    //等待卸货
    while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

    //熄灭红灯
    HAL_GPIO_WritePin(Red_GPIO_Port,Red_Pin,SET);
    printf("BACK_");
    //原路返回
    //后退
    CarBackStraight();

    //等待一根红线 （后退相反检测）
    CarBackTracking();

    //后退一段距离
    CarBackDistance(350);

    //第二步 右转弯
    CarRightTurn();

    //起步
    CarStart();
    Res_Rback();    //等待回归线上

    HAL_Delay(100);
    //巡线
    CarCallTracking();
    //CarTracking();
    //停
    CarStop();

    //点亮绿灯
    HAL_GPIO_WritePin(Green_GPIO_Port,Green_Pin,RESET);
    while(1);
}


void BaseThree(void)      //数字三
{
    uint8_t  photo_y = 0;
    //第一步
    //起步
    CarStart();

    //巡线  检测到第一个红线
    CarTracking();

    HAL_Delay(500);

    //巡线  检测到第二个红线
    CarTracking();
    //后退
    CarBackDistance(1500);

    //摄像头检测 数字 舵机确认位置

    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 65);//输出相机角度值     右
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 76);//输出相机角度值     左
    //判断是否在 这个路口 在左（1） 右（0） 不在（2）
    photo_y = Camera();
    if(photo_y == 1)       //在这个路口 左边
    {
        //第二步 左转弯
        CarLeftTurn();

        //第三步 巡线 检测病房入口             （未加入口后防抖动）！！！！！！！
        CarTracking();

        //第四步 停 点亮红灯 等待卸货
        //停
        CarStop();

        //点亮红灯


        //等待卸货
        while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

        //原路返回
        //后退
        CarBackStraight();

        //等待一根红线 （后退相反检测）
        CarBackTracking();

        //后退一段距离
        CarBackDistance(800);

        //第二步 左转弯
        CarRightTurn();

        //巡线
        CarTracking();

        //巡线
        CarTracking();

        //停
        CarStop();
    }else if (photo_y == 0) //在这个路口 右边
    {
        //第二步 右转弯
        CarRightTurn();

        //第三步 巡线 检测病房入口             （未加入口后防抖动）！！！！！！！
        CarTracking();

        //第四步 停 点亮红灯 等待卸货
        //停
        CarStop();

        //点亮红灯


        //等待卸货
        while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

        //原路返回
        //后退
        CarBackStraight();

        //等待一根红线 （后退相反检测）
        CarBackTracking();

        //后退一段距离
        CarBackDistance(800);

        //第二步 右转弯
        CarRightTurn();

        //巡线
        CarTracking();

        //巡线
        CarTracking();

        //停
        CarStop();
    } else if (photo_y == 2)
    {
        //起步
        CarStart();

        //后退
        CarBackDistance(800);

        //摄像头检测 数字


        //判断是在左 还是 在右 (1)左 (0)右
        if (CameraTwo())    //远端病房 左端
        {
            //左转弯
            CarLeftTurn();

            //循迹
            CarTracking();

            //后退一段距离
            CarBackDistance(800);

            //摄像头检测 数字

            //判断是在左 还是 在右 (1)左 (0)右
            if(Camera())           //在远端病房 最左端 左边
            {
                // 左转弯
                CarLeftTurn();

                // 巡线 检测病房入口             （未加入口后防抖动）！！！！！！！
                CarTracking();

                //第四步 停 点亮红灯 等待卸货
                //停
                CarStop();

                //点亮红灯


                //等待卸货
                while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

                //原路返回
                //后退
                CarBackStraight();

                //等待一根红线 （后退相反检测） 半根红线！！！！！！！！！
                CarBackTracking();

                //后退一段距离
                CarBackDistance(800);

                // 左转弯
                CarLeftTurn();

                //巡线                !!!!!只有半根线 另写函数 检测半根红线
                CarTracking();

                //后退
                CarBackDistance(800);

                //右转
                CarRightTurn();

                //循迹
                CarTracking();

                //循迹
                CarTracking();

                //循迹
                CarTracking();

                //停
                CarStop();

            }else           //在远端病房 最左端 右边
            {
                // 右转弯
                CarRightTurn();

                // 巡线 检测病房入口             （未加入口后防抖动）！！！！！！！
                CarTracking();

                //第四步 停 点亮红灯 等待卸货
                //停
                CarStop();

                //点亮红灯


                //等待卸货
                while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

                //原路返回
                //后退
                CarBackStraight();

                //等待一根红线 （后退相反检测） 半根红线！！！！！！！！！
                CarBackTracking();

                //后退一段距离
                CarBackDistance(800);

                // 右转弯
                CarRightTurn();

                //巡线                !!!!!只有半根线 另写函数 检测半根红线
                CarTracking();

                //后退
                CarBackDistance(800);

                //右转
                CarRightTurn();

                //循迹
                CarTracking();

                //循迹
                CarTracking();

                //循迹
                CarTracking();

                //停
                CarStop();
            }

        }else            //在远端病房 最右端
        {
            //右转弯
            CarRightTurn();

            //循迹
            CarTracking();

            //后退一段距离
            CarBackDistance(800);

            //摄像头检测 数字


            //判断是在左 还是 在右 (1)左 (0)右
            if(Camera())           //在远端病房 最右端 的左边
            {
                // 左转弯
                CarLeftTurn();

                // 巡线 检测病房入口             （未加入口后防抖动）！！！！！！！
                CarTracking();

                //第四步 停 点亮红灯 等待卸货
                //停
                CarStop();

                //点亮红灯


                //等待卸货
                while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

                //原路返回
                //后退
                CarBackStraight();

                //等待一根红线 （后退相反检测） 半根红线！！！！！！！！！
                CarBackTracking();

                //后退一段距离
                CarBackDistance(800);

                // 左转弯
                CarLeftTurn();

                //巡线                !!!!!只有半根线 另写函数 检测半根红线
                CarTracking();

                //后退
                CarBackDistance(800);

                //左转
                CarLeftTurn();

                //循迹
                CarTracking();

                //循迹
                CarTracking();

                //循迹
                CarTracking();

                //停
                CarStop();


            }
            else           //在远端病房 最右端 右边
            {
                // 右转弯
                CarRightTurn();

                // 巡线 检测病房入口             （未加入口后防抖动）！！！！！！！
                CarTracking();

                //第四步 停 点亮红灯 等待卸货
                //停
                CarStop();

                //点亮红灯


                //等待卸货
                while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

                //原路返回
                //后退
                CarBackStraight();

                //等待一根红线 （后退相反检测）半根红线！！！！！！！！！
                CarBackTracking();

                //后退一段距离
                CarBackDistance(800);

                // 右弯
                CarRightTurn();

                //巡线                !!!!!只有半根线 另写函数 检测半根红线
                CarTracking();

                //后退
                CarBackDistance(800);

                //左转
                CarLeftTurn();

                //循迹
                CarTracking();

                //循迹
                CarTracking();

                //循迹
                CarTracking();

                //停
                CarStop();
            }
        }
    }
}

void BaseFour(void)       //数字四
{

}

void BaseFive(void)       //数字五
{

}

void BaseSix(void)        //数字六
{

}

void BaseSeven(void)      //数字七
{

}

void BaseEight(void)      //数字八
{

}