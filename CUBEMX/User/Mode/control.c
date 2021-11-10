#include "control.h"
#include "math.h"
#include "tim.h"
#include "stm32f1xx_it.h"

unsigned int BMotorPulse,AMotorPulse;//全局变量,保存B电机脉冲数值



int SpeedTarget = 0;//全局变量,电机速度目标值
int MotorOutput;//电机输出

int ErrorPrev,PulsePrev;//上一次偏差值，上一次速度
int PwmBais,Pwm;//PWM增量， PWM总量
int Error,Pulse;//偏差
int MotorCouter;



void CarBackStraight(void);
void CarStop(void);




void GetMotorPulse(void)//读取电机B,A的脉冲
{
	
	BMotorPulse = (short)(__HAL_TIM_GET_COUNTER(&htim8));//获取B电机计数器值
	__HAL_TIM_SET_COUNTER(&htim8,0);//B电机计数器清零
	
	AMotorPulse = (short)(__HAL_TIM_GET_COUNTER(&htim5));//获取A电机计数器值
	__HAL_TIM_SET_COUNTER(&htim5,0);//A电机计数器清零
	
}

int SpeedInnerControl(int BPulse,int APulse,int Target)//B电机速度内环控制：BPulse实际速度，BTarget目标速度
{


	float fP = 11, fI = 0.35;//这里只用到PI， 参数由电机的种类和负载决定
	
	Pulse = (BPulse + APulse) / 2;//求A,B两电机速度的平均值
	Pulse = 0.7 * PulsePrev + 0.3 * Pulse;//低通滤波，使速度更平滑
	PulsePrev = Pulse;//保存前一次速度
	
	Error = Target - Pulse;//偏差 = 目标速度 - 实际速度
	PwmBais = fP * (Error - ErrorPrev) + fI * Error;//增量式PI控制器
	ErrorPrev = Error;//保存上一次偏差
	Pwm += PwmBais;//增量输出
	
	if(Pwm > 7200) Pwm = 7200;//限制上限， 防止超出PWM量程
	if(Pwm < 4200) Pwm = 4200;
	
	/******模拟示波器输出量设置**********/
//	OutData[0]=(float)BPulse;//速度实际值[红线]
//	OutData[3]=(float)APulse;//速度实际值[粉线]
//	OutData[1]=(float)Target ;//速度目标值[黄线]
//	OutData[2]=(float)Pwm;//PWM输出值[蓝线]
	
	return Pwm;//返回B电机输出值
	
}

void VoltageAndDirection(int Pwm)//设置电机B,A电压和方向             前进
{
	
		__HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_1,Pwm);
		__HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_2,0);//A电机

		__HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_3,Pwm);
		__HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_4,0);//B电机
	
}

void OpposeVoltageAndDirection(int Pwm)//设置电机B,A电压和方向        后退
{

        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_1,0);
        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_2,Pwm);//A电机

        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_3,0);
        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_4,Pwm);//B电机

}

void LeftDifferential(int Pwm)        //左差数
{

        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_1,Pwm);
        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_2,0);//A电机

        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_3,0);
        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_4,Pwm);//B电机
}

void RightDifferential(int Pwm)        //右差数
{

        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_1,0);
        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_2,Pwm);//A电机

        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_3,Pwm);
        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_4,0);//B电机
}


/*
*********************************************************************************************************
*	函 数 名: CarStart
*	功能说明: 小车起步
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarStart(void)
{
    CarStop();
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 70);//输出小车角度值 归正
    SpeedTarget = 350;                              //速度设置（巡线要快 抢时间）
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
    VoltageAndDirection(0);
    Pwm=0;                          //  电机停止
    ErrorPrev=0;
    PulsePrev=0;
    PwmBais=0;
    Error=0;
    Pulse=0;
    BMotorPulse = 0;
    AMotorPulse = 0;

    __HAL_TIM_SET_COUNTER(&htim3,0);//A电机计数器清零
    __HAL_TIM_SET_COUNTER(&htim5,0);//B电机计数器清零


    __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_1,1000000);
    __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_2,1000000);//A电机

    __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_3,1000000);
    __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_4,1000000);//B电机






}


/*
*********************************************************************************************************
*	函 数 名: CarBackDistance
*	功能说明: 小车后退一定距离
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarBackDistance(void)
{
    //第一步 停车
    CarStop();

    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 70);//输出小车角度值 回归平行\
    //第二步后退
    SpeedTarget = -100;                              //速度设置（转弯 速度慢） -400~-300
    MovingDirection = 2;                            //后退

    HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断   开启电机
    MotorCouter=0;

    while (MotorCouter > -5000)
    {
        printf("%d\r\n",MotorCouter);
    }


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
        }
                __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, angle);//输出小车角度值
    } while (state != 0x0f);
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
            case 4 :
                angle = 75;
                break;//左偏，向右修正
            case 8 :
                angle = 65;
                break;//右偏，向左修正

        }
                __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, angle);//输出小车角度值
    } while (state != 0x0f);                                                //判断是否检测到一根红线
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
    CarStart();
    do                                              //初步的巡线
    {
        state = InfraredScan();//红外扫描,获取角度值
        switch (state) {

            /****维持向前行驶****/
            case 4 :
                angle = 76;
                break;//左偏，向右修正

        }
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, angle);//输出小车角度值
    } while (state != 0x0f);

    SpeedTarget = -150;                              //速度设置（转弯 速度慢） -400~-300
    MovingDirection = 2;                            //后退
    HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断   开启电机
    HAL_Delay(250);    //后退延时一秒

    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 45);//向左转
    SpeedTarget = 100;                              //速度设置（转弯 速度慢）
    MovingDirection = 3;                            //左差数
    HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断   开启电机
    HAL_Delay(700);    //差数后延时一秒
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


    SpeedTarget = -150;                              //速度设置（转弯 速度慢） -400~-300
    MovingDirection = 2;                            //后退
    HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断   开启电机
    HAL_Delay(200);    //后退延时一秒

    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 125);//向左转
    SpeedTarget = 100;                              //速度设置（转弯 速度慢）
    MovingDirection = 3;                            //左差数
    HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断   开启电机
    HAL_Delay(1000);    //差数后延时一秒
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
    //第一步   起步
    CarStart();
    //巡线
    CarTracking();


    //后退
    CarBackDistance();


    //第二步 左转弯
    CarLeftTurn();

    SpeedTarget = 50;                              //速度设置（巡线要快 抢时间）
    MovingDirection = 1;                            //前进
    HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断   开启电机


    do                                              //初步的巡线
    {
        state = InfraredScan();//红外扫描,获取角度值

    } while (!(state == 8 || state == 16));
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 75);//输出小车角度值

    CarStart();
    HAL_Delay(10);

    //第三步 巡线 检测病房入口             （未加入口后防抖动）！！！！！！！
//    CarTracking();

    do                                              //初步的巡线
    {
        state = InfraredScan();//红外扫描,获取角度值
    } while (!(state == 0));



    //第四步 停 点亮红灯 等待卸货
    //停
    CarStop();



    //点亮红灯
    HAL_GPIO_WritePin(Red_GPIO_Port,Red_Pin,GPIO_PIN_RESET);

    //等待卸货
    while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == SET);

    //熄灭红灯
    HAL_GPIO_WritePin(Red_GPIO_Port,Red_Pin,GPIO_PIN_RESET);

    //原路返回
    //后退
    CarBackStraight();

    //等待一根红线 （后退相反检测）
    do                                              //初步的巡线
    {
        state = InfraredScan();//红外扫描,获取角度值
        switch (state) {

            /****维持向前行驶****/
            case 8 :
                angle = 75;
                break;//左灯灭右偏，向左修正，角度越小左转向
            case 4 :
                angle = 65;
                break;//右灯灭左偏，向右修正，角度越大右转向
                // /****维持****/
            case 16 :
                angle = 85;
                break;//左灯灭右偏，向左修正，角度越小左转向
            case 2 :
                angle = 55;
                break;//右灯灭左偏，向右修正，角度越大右转向
        }
                __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, angle);//输出小车角度值
    } while (state != 0x0f);

    //后退一段距离
    CarBackDistance();

    //第二步 左转弯
    CarLeftTurn();

    SpeedTarget = 50;                              //速度设置（巡线要快 抢时间）
    MovingDirection = 1;                            //前进
    HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断   开启电机


    do                                              //初步的巡线
    {
        state = InfraredScan();//红外扫描,获取角度值

    } while (!(state == 8 || state == 16));


    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 70);//输出小车角度值 归正
    SpeedTarget = 100;                              //速度设置（巡线要快 抢时间）
    MovingDirection = 1;                            //前进
    HAL_TIM_Base_Start_IT(&htim6);  // 开启定时器更新中断   开启电机

    //巡线
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
        }
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, angle);//输出小车角度值
    } while (!(state == 0));


    //停
    CarStop();
	
    //点亮绿灯
    HAL_GPIO_WritePin(Green_GPIO_Port,Green_Pin,GPIO_PIN_SET);


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
    //第一步   起步
    CarStart();
    //巡线
    CarTracking();

    //后退
    CarBackDistance();

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
    CarBackDistance();

    //第二步 右转弯
    CarRightTurn();


    //巡线
    CarTracking();

    //停
    CarStop();
}


void BaseThree(void)      //数字三
{
    //第一步
    //起步
    CarStart();

    //巡线  检测到第一个红线
    CarTracking();

    //巡线  检测到第二个红线
    CarTracking();

    //后退
    CarBackDistance();

    //摄像头检测 数字



    //判断是否在 这个路口 （1）在 （0）不在
    if(1)
    {
        //判断是在左 还是 在右 (1)左 (0)右         中部药房
        if(1)               //在左
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
            CarBackDistance();

            //第二步 左转弯
            CarRightTurn();

            //巡线
            CarTracking();

            //巡线
            CarTracking();

            //停
            CarStop();
        }
        else        //在右
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
            CarBackDistance();

            //第二步 右转弯
            CarRightTurn();

            //巡线
            CarTracking();

            //巡线
            CarTracking();

            //停
            CarStop();
        }

    }
    else        //在远端病房
    {
        //起步
        CarStart();

        //巡线
        CarTracking();

        //后退
        CarBackDistance();

        //摄像头检测 数字


        //判断是在左 还是 在右 (1)左 (0)右
        if(1)       //远端病房 左端
        {
            //左转弯
            CarLeftTurn();

            //循迹
            CarTracking();

            //后退一段距离
            CarBackDistance();

            //摄像头检测 数字


            //判断是在左 还是 在右 (1)左 (0)右
            if(1)           //在远端病房 最左端 左边
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
                CarBackDistance();

                // 左转弯
                CarLeftTurn();

                //巡线                !!!!!只有半根线 另写函数 检测半根红线
                CarTracking();

                //后退
                CarBackDistance();

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
            else           //在远端病房 最左端 右边
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
                CarBackDistance();

                // 右转弯
                CarRightTurn();

                //巡线                !!!!!只有半根线 另写函数 检测半根红线
                CarTracking();

                //后退
                CarBackDistance();

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

        }
        else            //在远端病房 最右端
        {
            //右转弯
            CarRightTurn();

            //循迹
            CarTracking();

            //后退一段距离
            CarBackDistance();

            //摄像头检测 数字


            //判断是在左 还是 在右 (1)左 (0)右
            if(1)           //在远端病房 最右端 的左边
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
                CarBackDistance();

                // 左转弯
                CarLeftTurn();

                //巡线                !!!!!只有半根线 另写函数 检测半根红线
                CarTracking();

                //后退
                CarBackDistance();

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
                CarBackDistance();

                // 右弯
                CarRightTurn();

                //巡线                !!!!!只有半根线 另写函数 检测半根红线
                CarTracking();

                //后退
                CarBackDistance();

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