#include "control.h"
#include "math.h"
#include "tim.h"
#include "stm32f1xx_it.h"

unsigned int BMotorPulse,AMotorPulse;//ȫ�ֱ���,����B���������ֵ



int SpeedTarget = 0;//ȫ�ֱ���,����ٶ�Ŀ��ֵ
int MotorOutput;//������

int ErrorPrev,PulsePrev;//��һ��ƫ��ֵ����һ���ٶ�
int PwmBais,Pwm;//PWM������ PWM����
int Error,Pulse;//ƫ��
int MotorCouter;



void CarBackStraight(void);
void CarStop(void);




void GetMotorPulse(void)//��ȡ���B,A������
{
	
	BMotorPulse = (short)(__HAL_TIM_GET_COUNTER(&htim8));//��ȡB���������ֵ
	__HAL_TIM_SET_COUNTER(&htim8,0);//B�������������
	
	AMotorPulse = (short)(__HAL_TIM_GET_COUNTER(&htim5));//��ȡA���������ֵ
	__HAL_TIM_SET_COUNTER(&htim5,0);//A�������������
	
}

int SpeedInnerControl(int BPulse,int APulse,int Target)//B����ٶ��ڻ����ƣ�BPulseʵ���ٶȣ�BTargetĿ���ٶ�
{


	float fP = 11, fI = 0.35;//����ֻ�õ�PI�� �����ɵ��������͸��ؾ���
	
	Pulse = (BPulse + APulse) / 2;//��A,B������ٶȵ�ƽ��ֵ
	Pulse = 0.7 * PulsePrev + 0.3 * Pulse;//��ͨ�˲���ʹ�ٶȸ�ƽ��
	PulsePrev = Pulse;//����ǰһ���ٶ�
	
	Error = Target - Pulse;//ƫ�� = Ŀ���ٶ� - ʵ���ٶ�
	PwmBais = fP * (Error - ErrorPrev) + fI * Error;//����ʽPI������
	ErrorPrev = Error;//������һ��ƫ��
	Pwm += PwmBais;//�������
	
	if(Pwm > 7200) Pwm = 7200;//�������ޣ� ��ֹ����PWM����
	if(Pwm < 4200) Pwm = 4200;
	
	/******ģ��ʾ�������������**********/
//	OutData[0]=(float)BPulse;//�ٶ�ʵ��ֵ[����]
//	OutData[3]=(float)APulse;//�ٶ�ʵ��ֵ[����]
//	OutData[1]=(float)Target ;//�ٶ�Ŀ��ֵ[����]
//	OutData[2]=(float)Pwm;//PWM���ֵ[����]
	
	return Pwm;//����B������ֵ
	
}

void VoltageAndDirection(int Pwm)//���õ��B,A��ѹ�ͷ���             ǰ��
{
	
		__HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_1,Pwm);
		__HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_2,0);//A���

		__HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_3,Pwm);
		__HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_4,0);//B���
	
}

void OpposeVoltageAndDirection(int Pwm)//���õ��B,A��ѹ�ͷ���        ����
{

        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_1,0);
        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_2,Pwm);//A���

        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_3,0);
        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_4,Pwm);//B���

}

void LeftDifferential(int Pwm)        //�����
{

        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_1,Pwm);
        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_2,0);//A���

        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_3,0);
        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_4,Pwm);//B���
}

void RightDifferential(int Pwm)        //�Ҳ���
{

        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_1,0);
        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_2,Pwm);//A���

        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_3,Pwm);
        __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_4,0);//B���
}


/*
*********************************************************************************************************
*	�� �� ��: CarStart
*	����˵��: С����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CarStart(void)
{
    CarStop();
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 70);//���С���Ƕ�ֵ ����
    SpeedTarget = 350;                              //�ٶ����ã�Ѳ��Ҫ�� ��ʱ�䣩
    MovingDirection = 1;                            //ǰ��
    HAL_TIM_Base_Start_IT(&htim6);  // ������ʱ�������ж�   �������
}


/*
*********************************************************************************************************
*	�� �� ��: CarStop
*	����˵��: ͣ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CarStop(void)
{
    HAL_TIM_Base_Stop_IT(&htim6);   //  �رն�ʱ�������ж�    �رյ��
    VoltageAndDirection(0);
    Pwm=0;                          //  ���ֹͣ
    ErrorPrev=0;
    PulsePrev=0;
    PwmBais=0;
    Error=0;
    Pulse=0;
    BMotorPulse = 0;
    AMotorPulse = 0;

    __HAL_TIM_SET_COUNTER(&htim3,0);//A�������������
    __HAL_TIM_SET_COUNTER(&htim5,0);//B�������������


    __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_1,1000000);
    __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_2,1000000);//A���

    __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_3,1000000);
    __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_4,1000000);//B���






}


/*
*********************************************************************************************************
*	�� �� ��: CarBackDistance
*	����˵��: С������һ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CarBackDistance(void)
{
    //��һ�� ͣ��
    CarStop();

    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 70);//���С���Ƕ�ֵ �ع�ƽ��\
    //�ڶ�������
    SpeedTarget = -100;                              //�ٶ����ã�ת�� �ٶ����� -400~-300
    MovingDirection = 2;                            //����

    HAL_TIM_Base_Start_IT(&htim6);  // ������ʱ�������ж�   �������
    MotorCouter=0;

    while (MotorCouter > -5000)
    {
        printf("%d\r\n",MotorCouter);
    }


    //������ ͣ��
    CarStop();

}


/*
*********************************************************************************************************
*	�� �� ��: CarBackStraight
*	����˵��: С��ֱ�ߺ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CarBackStraight(void)
{
    SpeedTarget = -500;                              //�ٶ����ã�Ѳ��Ҫ�� ��ʱ�䣩
    MovingDirection = 2;                            //����
    HAL_TIM_Base_Start_IT(&htim6);  // ������ʱ�������ж�   �������
}


/*
*********************************************************************************************************
*	�� �� ��: CarTracking
*	����˵��: ������Ѳ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CarTracking(void)
{
    do                                              //������Ѳ��
    {
        state = InfraredScan();//����ɨ��,��ȡ�Ƕ�ֵ
        switch (state) {

            /****ά����ǰ��ʻ****/
            case 8 :
                angle = 65;
                break;//�������ƫ�������������Ƕ�ԽС��ת��
            case 4 :
                angle = 75;
                break;//�ҵ�����ƫ�������������Ƕ�Խ����ת��
                // /****ά��****/
            case 16 :
                angle = 55;
                break;//�������ƫ�������������Ƕ�ԽС��ת��
            case 2 :
                angle = 85;
                break;//�ҵ�����ƫ�������������Ƕ�Խ����ת��
        }
                __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, angle);//���С���Ƕ�ֵ
    } while (state != 0x0f);
}


/*
*********************************************************************************************************
*	�� �� ��: CarBackTracking
*	����˵��: ���˵�Ѳ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CarBackTracking(void)
{
    //������ Ѳ�� ��ⲡ�����             ��δ����ں����������������������
    do                                              //������Ѳ��
    {
        state = InfraredScan();//����ɨ��,��ȡ�Ƕ�ֵ
        switch (state) {

            /****ά����ǰ��ʻ****/
            case 4 :
                angle = 75;
                break;//��ƫ����������
            case 8 :
                angle = 65;
                break;//��ƫ����������

        }
                __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, angle);//���С���Ƕ�ֵ
    } while (state != 0x0f);                                                //�ж��Ƿ��⵽һ������
}


/*
*********************************************************************************************************
*	�� �� ��: CarLeftTurn
*	����˵��: С����ת
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CarLeftTurn(void)      //С����ת  �����˼Ӳ�����
{
    CarStart();
    do                                              //������Ѳ��
    {
        state = InfraredScan();//����ɨ��,��ȡ�Ƕ�ֵ
        switch (state) {

            /****ά����ǰ��ʻ****/
            case 4 :
                angle = 76;
                break;//��ƫ����������

        }
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, angle);//���С���Ƕ�ֵ
    } while (state != 0x0f);

    SpeedTarget = -150;                              //�ٶ����ã�ת�� �ٶ����� -400~-300
    MovingDirection = 2;                            //����
    HAL_TIM_Base_Start_IT(&htim6);  // ������ʱ�������ж�   �������
    HAL_Delay(250);    //������ʱһ��

    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 45);//����ת
    SpeedTarget = 100;                              //�ٶ����ã�ת�� �ٶ�����
    MovingDirection = 3;                            //�����
    HAL_TIM_Base_Start_IT(&htim6);  // ������ʱ�������ж�   �������
    HAL_Delay(700);    //��������ʱһ��
}

/*
*********************************************************************************************************
*	�� �� ��: CarRightTurn
*	����˵��: С����ת
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CarRightTurn(void)      //С����ת  �����˼Ӳ�����
{


    SpeedTarget = -150;                              //�ٶ����ã�ת�� �ٶ����� -400~-300
    MovingDirection = 2;                            //����
    HAL_TIM_Base_Start_IT(&htim6);  // ������ʱ�������ж�   �������
    HAL_Delay(200);    //������ʱһ��

    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 125);//����ת
    SpeedTarget = 100;                              //�ٶ����ã�ת�� �ٶ�����
    MovingDirection = 3;                            //�����
    HAL_TIM_Base_Start_IT(&htim6);  // ������ʱ�������ж�   �������
    HAL_Delay(1000);    //��������ʱһ��
}


/*
*********************************************************************************************************
*	�� �� ��: BaseOne
*	����˵��: ��������һ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void BaseOne(void)          //����һ
{
    //��һ��   ��
    CarStart();
    //Ѳ��
    CarTracking();


    //����
    CarBackDistance();


    //�ڶ��� ��ת��
    CarLeftTurn();

    SpeedTarget = 50;                              //�ٶ����ã�Ѳ��Ҫ�� ��ʱ�䣩
    MovingDirection = 1;                            //ǰ��
    HAL_TIM_Base_Start_IT(&htim6);  // ������ʱ�������ж�   �������


    do                                              //������Ѳ��
    {
        state = InfraredScan();//����ɨ��,��ȡ�Ƕ�ֵ

    } while (!(state == 8 || state == 16));
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 75);//���С���Ƕ�ֵ

    CarStart();
    HAL_Delay(10);

    //������ Ѳ�� ��ⲡ�����             ��δ����ں����������������������
//    CarTracking();

    do                                              //������Ѳ��
    {
        state = InfraredScan();//����ɨ��,��ȡ�Ƕ�ֵ
    } while (!(state == 0));



    //���Ĳ� ͣ ������� �ȴ�ж��
    //ͣ
    CarStop();



    //�������
    HAL_GPIO_WritePin(Red_GPIO_Port,Red_Pin,GPIO_PIN_RESET);

    //�ȴ�ж��
    while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == SET);

    //Ϩ����
    HAL_GPIO_WritePin(Red_GPIO_Port,Red_Pin,GPIO_PIN_RESET);

    //ԭ·����
    //����
    CarBackStraight();

    //�ȴ�һ������ �������෴��⣩
    do                                              //������Ѳ��
    {
        state = InfraredScan();//����ɨ��,��ȡ�Ƕ�ֵ
        switch (state) {

            /****ά����ǰ��ʻ****/
            case 8 :
                angle = 75;
                break;//�������ƫ�������������Ƕ�ԽС��ת��
            case 4 :
                angle = 65;
                break;//�ҵ�����ƫ�������������Ƕ�Խ����ת��
                // /****ά��****/
            case 16 :
                angle = 85;
                break;//�������ƫ�������������Ƕ�ԽС��ת��
            case 2 :
                angle = 55;
                break;//�ҵ�����ƫ�������������Ƕ�Խ����ת��
        }
                __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, angle);//���С���Ƕ�ֵ
    } while (state != 0x0f);

    //����һ�ξ���
    CarBackDistance();

    //�ڶ��� ��ת��
    CarLeftTurn();

    SpeedTarget = 50;                              //�ٶ����ã�Ѳ��Ҫ�� ��ʱ�䣩
    MovingDirection = 1;                            //ǰ��
    HAL_TIM_Base_Start_IT(&htim6);  // ������ʱ�������ж�   �������


    do                                              //������Ѳ��
    {
        state = InfraredScan();//����ɨ��,��ȡ�Ƕ�ֵ

    } while (!(state == 8 || state == 16));


    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 70);//���С���Ƕ�ֵ ����
    SpeedTarget = 100;                              //�ٶ����ã�Ѳ��Ҫ�� ��ʱ�䣩
    MovingDirection = 1;                            //ǰ��
    HAL_TIM_Base_Start_IT(&htim6);  // ������ʱ�������ж�   �������

    //Ѳ��
    do                                              //������Ѳ��
    {
        state = InfraredScan();//����ɨ��,��ȡ�Ƕ�ֵ
        switch (state) {

            /****ά����ǰ��ʻ****/
            case 8 :
                angle = 65;
                break;//�������ƫ�������������Ƕ�ԽС��ת��
            case 4 :
                angle = 75;
                break;//�ҵ�����ƫ�������������Ƕ�Խ����ת��
                // /****ά��****/
            case 16 :
                angle = 55;
                break;//�������ƫ�������������Ƕ�ԽС��ת��
            case 2 :
                angle = 85;
                break;//�ҵ�����ƫ�������������Ƕ�Խ����ת��
        }
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, angle);//���С���Ƕ�ֵ
    } while (!(state == 0));


    //ͣ
    CarStop();
	
    //�����̵�
    HAL_GPIO_WritePin(Green_GPIO_Port,Green_Pin,GPIO_PIN_SET);


}

/*
*********************************************************************************************************
*	�� �� ��: BaseTwo
*	����˵��: ���������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void BaseTwo(void)        //���ֶ�
{
    //��һ��   ��
    CarStart();
    //Ѳ��
    CarTracking();

    //����
    CarBackDistance();

    //�ڶ��� ��ת��
    CarRightTurn();

    //������ Ѳ�� ��ⲡ�����             ��δ����ں����������������������
    CarTracking();

    //���Ĳ� ͣ ������� �ȴ�ж��
    //ͣ
    CarStop();

    //�������


    //�ȴ�ж��
    while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

    //ԭ·����
    //����
    CarBackStraight();

    //�ȴ�һ������ �������෴��⣩
    CarBackTracking();

    //����һ�ξ���
    CarBackDistance();

    //�ڶ��� ��ת��
    CarRightTurn();


    //Ѳ��
    CarTracking();

    //ͣ
    CarStop();
}


void BaseThree(void)      //������
{
    //��һ��
    //��
    CarStart();

    //Ѳ��  ��⵽��һ������
    CarTracking();

    //Ѳ��  ��⵽�ڶ�������
    CarTracking();

    //����
    CarBackDistance();

    //����ͷ��� ����



    //�ж��Ƿ��� ���·�� ��1���� ��0������
    if(1)
    {
        //�ж������� ���� ���� (1)�� (0)��         �в�ҩ��
        if(1)               //����
        {
            //�ڶ��� ��ת��
            CarLeftTurn();

            //������ Ѳ�� ��ⲡ�����             ��δ����ں����������������������
            CarTracking();

            //���Ĳ� ͣ ������� �ȴ�ж��
            //ͣ
            CarStop();

            //�������


            //�ȴ�ж��
            while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

            //ԭ·����
            //����
            CarBackStraight();

            //�ȴ�һ������ �������෴��⣩
            CarBackTracking();

            //����һ�ξ���
            CarBackDistance();

            //�ڶ��� ��ת��
            CarRightTurn();

            //Ѳ��
            CarTracking();

            //Ѳ��
            CarTracking();

            //ͣ
            CarStop();
        }
        else        //����
        {
            //�ڶ��� ��ת��
            CarRightTurn();

            //������ Ѳ�� ��ⲡ�����             ��δ����ں����������������������
            CarTracking();

            //���Ĳ� ͣ ������� �ȴ�ж��
            //ͣ
            CarStop();

            //�������


            //�ȴ�ж��
            while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

            //ԭ·����
            //����
            CarBackStraight();

            //�ȴ�һ������ �������෴��⣩
            CarBackTracking();

            //����һ�ξ���
            CarBackDistance();

            //�ڶ��� ��ת��
            CarRightTurn();

            //Ѳ��
            CarTracking();

            //Ѳ��
            CarTracking();

            //ͣ
            CarStop();
        }

    }
    else        //��Զ�˲���
    {
        //��
        CarStart();

        //Ѳ��
        CarTracking();

        //����
        CarBackDistance();

        //����ͷ��� ����


        //�ж������� ���� ���� (1)�� (0)��
        if(1)       //Զ�˲��� ���
        {
            //��ת��
            CarLeftTurn();

            //ѭ��
            CarTracking();

            //����һ�ξ���
            CarBackDistance();

            //����ͷ��� ����


            //�ж������� ���� ���� (1)�� (0)��
            if(1)           //��Զ�˲��� ����� ���
            {
                // ��ת��
                CarLeftTurn();

                // Ѳ�� ��ⲡ�����             ��δ����ں����������������������
                CarTracking();

                //���Ĳ� ͣ ������� �ȴ�ж��
                //ͣ
                CarStop();

                //�������


                //�ȴ�ж��
                while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

                //ԭ·����
                //����
                CarBackStraight();

                //�ȴ�һ������ �������෴��⣩ ������ߣ�����������������
                CarBackTracking();

                //����һ�ξ���
                CarBackDistance();

                // ��ת��
                CarLeftTurn();

                //Ѳ��                !!!!!ֻ�а���� ��д���� ���������
                CarTracking();

                //����
                CarBackDistance();

                //��ת
                CarRightTurn();

                //ѭ��
                CarTracking();

                //ѭ��
                CarTracking();

                //ѭ��
                CarTracking();

                //ͣ
                CarStop();


            }
            else           //��Զ�˲��� ����� �ұ�
            {
                // ��ת��
                CarRightTurn();

                // Ѳ�� ��ⲡ�����             ��δ����ں����������������������
                CarTracking();

                //���Ĳ� ͣ ������� �ȴ�ж��
                //ͣ
                CarStop();

                //�������


                //�ȴ�ж��
                while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

                //ԭ·����
                //����
                CarBackStraight();

                //�ȴ�һ������ �������෴��⣩ ������ߣ�����������������
                CarBackTracking();

                //����һ�ξ���
                CarBackDistance();

                // ��ת��
                CarRightTurn();

                //Ѳ��                !!!!!ֻ�а���� ��д���� ���������
                CarTracking();

                //����
                CarBackDistance();

                //��ת
                CarRightTurn();

                //ѭ��
                CarTracking();

                //ѭ��
                CarTracking();

                //ѭ��
                CarTracking();

                //ͣ
                CarStop();
            }

        }
        else            //��Զ�˲��� ���Ҷ�
        {
            //��ת��
            CarRightTurn();

            //ѭ��
            CarTracking();

            //����һ�ξ���
            CarBackDistance();

            //����ͷ��� ����


            //�ж������� ���� ���� (1)�� (0)��
            if(1)           //��Զ�˲��� ���Ҷ� �����
            {
                // ��ת��
                CarLeftTurn();

                // Ѳ�� ��ⲡ�����             ��δ����ں����������������������
                CarTracking();

                //���Ĳ� ͣ ������� �ȴ�ж��
                //ͣ
                CarStop();

                //�������


                //�ȴ�ж��
                while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

                //ԭ·����
                //����
                CarBackStraight();

                //�ȴ�һ������ �������෴��⣩ ������ߣ�����������������
                CarBackTracking();

                //����һ�ξ���
                CarBackDistance();

                // ��ת��
                CarLeftTurn();

                //Ѳ��                !!!!!ֻ�а���� ��д���� ���������
                CarTracking();

                //����
                CarBackDistance();

                //��ת
                CarLeftTurn();

                //ѭ��
                CarTracking();

                //ѭ��
                CarTracking();

                //ѭ��
                CarTracking();

                //ͣ
                CarStop();


            }
            else           //��Զ�˲��� ���Ҷ� �ұ�
            {
                // ��ת��
                CarRightTurn();

                // Ѳ�� ��ⲡ�����             ��δ����ں����������������������
                CarTracking();

                //���Ĳ� ͣ ������� �ȴ�ж��
                //ͣ
                CarStop();

                //�������


                //�ȴ�ж��
                while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

                //ԭ·����
                //����
                CarBackStraight();

                //�ȴ�һ������ �������෴��⣩������ߣ�����������������
                CarBackTracking();

                //����һ�ξ���
                CarBackDistance();

                // ����
                CarRightTurn();

                //Ѳ��                !!!!!ֻ�а���� ��д���� ���������
                CarTracking();

                //����
                CarBackDistance();

                //��ת
                CarLeftTurn();

                //ѭ��
                CarTracking();

                //ѭ��
                CarTracking();

                //ѭ��
                CarTracking();

                //ͣ
                CarStop();
            }
        }

    }
}

void BaseFour(void)       //������
{

}

void BaseFive(void)       //������
{

}

void BaseSix(void)        //������
{

}

void BaseSeven(void)      //������
{

}

void BaseEight(void)      //���ְ�
{

}