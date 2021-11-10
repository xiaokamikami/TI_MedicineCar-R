#include "control.h"
#include "math.h"
#include "tim.h"
#include "stm32f1xx_it.h"
#include "OpenmvData.h"
#include "gpio.h"

unsigned int BMotorPulse,AMotorPulse;//ȫ�ֱ���,����B���������ֵ
void CarTracking(void);
void CarBackTracking(void);

void CarBackDistance(uint16_t tag);
int SpeedTarget = 0;//ȫ�ֱ���,����ٶ�Ŀ��ֵ

int ErrorPrev,PulsePrev;//��һ��ƫ��ֵ����һ���ٶ�
int PwmBais,Pwm;//PWM������ PWM����
int Error,Pulse;//ƫ��
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
            HAL_GPIO_WritePin(EN1_GPIO_Port,EN1_Pin,GPIO_PIN_RESET);	//���1��ת
            HAL_GPIO_WritePin(EN2_GPIO_Port,EN2_Pin,GPIO_PIN_SET);
        }
        else if(mode ==2)
        {
            HAL_GPIO_WritePin(EN1_GPIO_Port,EN1_Pin,GPIO_PIN_SET);	//���2��ת
            HAL_GPIO_WritePin(EN2_GPIO_Port,EN2_Pin,GPIO_PIN_RESET);
        }
    }
    else if(Motox == 2)
    {
        if(mode == 1)
        {
			HAL_GPIO_WritePin(EN3_GPIO_Port,EN3_Pin,GPIO_PIN_RESET);	//���2��ת
			HAL_GPIO_WritePin(EN4_GPIO_Port,EN4_Pin,GPIO_PIN_SET);
        }
        else if(mode ==2)
        {
            HAL_GPIO_WritePin(EN3_GPIO_Port,EN3_Pin,GPIO_PIN_SET);	//���2��ת
            HAL_GPIO_WritePin(EN4_GPIO_Port,EN4_Pin,GPIO_PIN_RESET);
        }
    }

}

void GetMotorPulse(void)//��ȡ���B,A������
{
	
	BMotorPulse = (short)(__HAL_TIM_GET_COUNTER(&htim4));//��ȡB���������ֵ
	__HAL_TIM_SET_COUNTER(&htim4,0);//B�������������
	
	AMotorPulse = (short)(__HAL_TIM_GET_COUNTER(&htim2));//��ȡA���������ֵ
	__HAL_TIM_SET_COUNTER(&htim2,0);//A�������������
	
}

int SpeedInnerControl1(int BPulse,int Target)//B����ٶ��ڻ����ƣ�BPulseʵ���ٶȣ�BTargetĿ���ٶ�
{


	float fP = 3, fI = 0.1;//����ֻ�õ�PI�� �����ɵ��������͸��ؾ���
	
	Pulse = (BPulse + BPulse) / 2;//��A,B������ٶȵ�ƽ��ֵ
	Pulse = 0.7 * PulsePrev + 0.3 * Pulse;//��ͨ�˲���ʹ�ٶȸ�ƽ��
	PulsePrev = Pulse;//����ǰһ���ٶ�
	
	Error = Target - Pulse;//ƫ�� = Ŀ���ٶ� - ʵ���ٶ�
	PwmBais = fP * (Error - ErrorPrev) + fI * Error;//����ʽPI������
	ErrorPrev = Error;//������һ��ƫ��
	Pwm += PwmBais;//�������
	
	if(Pwm > 2200) Pwm = 2200;//�������ޣ� ��ֹ����PWM����
	if(Pwm < 1000) Pwm = 1000;
	
	/******ģ��ʾ�������������**********/
//	OutData[0]=(float)BPulse;//�ٶ�ʵ��ֵ[����]
//	OutData[3]=(float)APulse;//�ٶ�ʵ��ֵ[����]
//	OutData[1]=(float)Target ;//�ٶ�Ŀ��ֵ[����]
//	OutData[2]=(float)Pwm;//PWM���ֵ[����]
	
	return Pwm;//����B������ֵ
	
}
int SpeedInnerControl2(int APulse,int Target)//B����ٶ��ڻ����ƣ�BPulseʵ���ٶȣ�BTargetĿ���ٶ�
{


    float fP = 3, fI = 0.1;//����ֻ�õ�PI�� �����ɵ��������͸��ؾ���

    Pulse = (APulse + APulse) / 2;//��A,B������ٶȵ�ƽ��ֵ
    Pulse = 0.7 * PulsePrev + 0.3 * Pulse;//��ͨ�˲���ʹ�ٶȸ�ƽ��
    PulsePrev = Pulse;//����ǰһ���ٶ�

    Error = Target - Pulse;//ƫ�� = Ŀ���ٶ� - ʵ���ٶ�
    PwmBais = fP * (Error - ErrorPrev) + fI * Error;//����ʽPI������
    ErrorPrev = Error;//������һ��ƫ��
    Pwm += PwmBais;//�������

    if(Pwm > 2200) Pwm = 2200;//�������ޣ� ��ֹ����PWM����
    if(Pwm < 1000) Pwm = 1000;

    /******ģ��ʾ�������������**********/
//	OutData[0]=(float)BPulse;//�ٶ�ʵ��ֵ[����]
//	OutData[3]=(float)APulse;//�ٶ�ʵ��ֵ[����]
//	OutData[1]=(float)Target ;//�ٶ�Ŀ��ֵ[����]
//	OutData[2]=(float)Pwm;//PWM���ֵ[����]

    return Pwm;//����B������ֵ

}
void VoltageAndDirection(int Pwm1,int Pwm2)//���õ��B,A��ѹ�ͷ���             ǰ��
{
	
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1,Pwm1);
         Motor_dict(1,1);

		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,Pwm2);
         Motor_dict(2,1);
	
}

void OpposeVoltageAndDirection(int Pwm1,int Pwm2)//���õ��B,A��ѹ�ͷ���        ����
{

        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1,Pwm1);
         Motor_dict(1,2);

        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,Pwm1);
        Motor_dict(2,2);

}

void LeftDifferential(int Pwm1,int Pwm2)        //�����
{
            __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1,Pwm1);
    Motor_dict(2,2);

    Motor_dict(1,1);
            __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,Pwm2);//B���
}

void RightDifferential(int Pwm1,int Pwm2)        //�Ҳ���
{
            __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1,Pwm1);//A���
    Motor_dict(1,2);

    Motor_dict(2,1);
            __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,Pwm2);



}
void Res_Rback(void)        //ʹС���ص�����
{
    do                    //���
    {
        state = InfraredScan();//����ɨ��,��ȡ�Ƕ�ֵ

    } while (state <= 0);
}
/*
*********************************************************************************************************
*	�� �� ��: Camera
*	����˵��: ����ͷ����Ƿ������·�� ����� �� ��1�� �ң�0��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int Camera(void)
{
    //���ת�� �� ��������Ƿ� �����
    //���

    //����ͷ���
    while(!(OpenmvData()));

    //�ж��Ƿ��� ��ص�����
    if(Xdate == TempXdate)
    {
        return 1;
    }

    // ���ת�� �� ��������Ƿ� ���Ҳ�
    //���

    //����ͷ���
    while(!(OpenmvData()));

    //�ж��Ƿ��� ��ص�����
    if(Xdate == TempXdate)
    {
        return 0;
    } else
    {
        return 2; //�������·��
    }


}


/*
*********************************************************************************************************
*	�� �� ��: CameraTwo
*	����˵��: �ڶ���·�� 4��ͼƬ 4��λ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int CameraTwo(void)
{
    //���ת�� ��2 ��������Ƿ� �����
    //���

    //����ͷ���
    while(!(OpenmvData()));

    //�ж��Ƿ��� ��ص�����
    if(Xdate == TempXdate)
    {
        return 1;
    }

    //���ת�� ��1 ��������Ƿ� �����
    //���

    //����ͷ���
    while(!(OpenmvData()));

    //�ж��Ƿ��� ��ص�����
    if(Xdate == TempXdate)
    {
        return 1;
    }

    // ���ת�� ��1 ��������Ƿ� ���Ҳ�
    //���

    //����ͷ���
    while(!(OpenmvData()));

    //�ж��Ƿ��� ��ص�����
    if(Xdate == TempXdate)
    {
        return 0;
    }

    // ���ת�� ��2 ��������Ƿ� ���Ҳ�
    //���

    //����ͷ���
    while(!(OpenmvData()));

    //�ж��Ƿ��� ��ص�����
    if(Xdate == TempXdate)
    {
        return 0;
    }


}



/*
*********************************************************************************************************
*	�� �� ��: CarStart
*	����˵��: ��ʼ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CarStart(void)
{
    CarStop();
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 75);//���С���Ƕ�ֵ ����
    SpeedTarget = 200;                              //�ٶ����ã�Ѳ��Ҫ�� ��ʱ�䣩
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
    VoltageAndDirection(0,0);
    Pwm=0;                          //  ���ֹͣ
    ErrorPrev=0;
    PulsePrev=0;
    PwmBais=0;
    Error=0;
    Pulse=0;
    BMotorPulse = 0;
    AMotorPulse = 0;
    MotorCouter1 = 0,MotorCouter2 = 0;
    __HAL_TIM_SET_COUNTER(&htim2,0);//A�������������
    __HAL_TIM_SET_COUNTER(&htim4,0);//B�������������


    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1,0);
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,0);//A���

    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3,0);
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4,0);//B���

}


/*
*********************************************************************************************************
*	�� �� ��: CarOver
*	����˵��: �����û���ſ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CarOver(void)
{

        do                                              //�����û���ſ�
        {
            state = InfraredScan();//����ɨ��,��ȡ�Ƕ�ֵ
        } while (!(state < 0));

}


/*
*********************************************************************************************************
*	�� �� ��: CarBackDistance
*	����˵��: С������һ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CarBackDistance(uint16_t tag)
{   //����ǿת
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 70);//���С���Ƕ�ֵ �ع�ƽ��
    SpeedTarget = 300;                              //�ٶ����ã�ת�� �ٶ����� -400~-300

    MotorCouter1=0,MotorCouter2= 0;
    while ( 1)
    {

        MovingDirection = 2;
        if ( MotorCouter1 >tag && MotorCouter2 > tag)
        {break;}
        //printf("%d\r\n",MotorCouter);
        state = InfraredScan();//����ɨ��,��ȡ�Ƕ�ֵ
        switch (state) {
            /****ά����ǰ��ʻ****/
            case 4 :
                angle = 75;
                break;//��ƫ����������
            case 8 :
                angle = 65;
                break;//��ƫ����������
            case 16:
                angle = 85;
                break;
            case 2:
                angle = 55;
                break;
        }
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, angle);//���С���Ƕ�ֵ
    }
    MovingDirection = 1;                            //����
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
            case 1 :
                angle = 100;
                break;//�ҵ�����ƫ�������������Ƕ�Խ����ת��
            case 32 :
                angle = 50;
                break;//�ҵ�����ƫ�������������Ƕ�Խ����ת��
        }
                __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, angle);//���С���Ƕ�ֵ
    } while (state != 0x0f);
    //MovingDirection = 2;                            //��һ�㳵
    //HAL_Delay(400);
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
            case 1 :
                angle = 50;
                break;//��ƫ����������
            case 2:
                angle = 60;
                break;//��ƫ����������
            case 4:
                angle = 70;
                break;
            case 8:
                angle = 80;
                break;
            case 16 :
                angle = 90;
                break;//�ҵ�����ƫ�������������Ƕ�Խ����ת��
            case 32 :
                angle = 100;
                break;//�ҵ�����ƫ�������������Ƕ�Խ����ת��

        }
                __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, angle);//���С���Ƕ�ֵ
    } while (state != 0x0f);                                                //�ж��Ƿ��⵽һ������
}

/*
*********************************************************************************************************
*	�� �� ��: CarCallTracking
*	����˵��: С�����ص�Ѳ�� Ѱ�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CarCallTracking(void)
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
            case 1 :
                angle = 100;
                break;//�ҵ�����ƫ�������������Ƕ�Խ����ת��
            case 32 :
                angle = 50;
                break;//�ҵ�����ƫ�������������Ƕ�Խ����ת��
        }
                __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, angle);//���С���Ƕ�ֵ
    } while (Idata <2);
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
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 65);//����ת
    HAL_Delay(100);
    SpeedTarget = 120;                              //�ٶ����ã�ת�� �ٶ�����
    MovingDirection = 4;                            //�����
    HAL_TIM_Base_Start_IT(&htim6);  // ������ʱ�������ж�   �������
    MotorCouter1 = 0,MotorCouter2 = 0;

    while (1 )
    {
        MovingDirection = 4;                            //�����
        printf("%d\r\n",MotorCouter2);
        if(MotorCouter1 >3600 && MotorCouter2 > 3600)
        {break;}
    }

    MovingDirection = 1;
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

    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 100);//����ת
    HAL_Delay(100);
    SpeedTarget = 120;                              //�ٶ����ã�ת�� �ٶ�����
    MovingDirection = 3;                            //�����
    HAL_TIM_Base_Start_IT(&htim6);  // ������ʱ�������ж�   �������
    MotorCouter1 = 0,MotorCouter2 = 0;

    while (1 )
    {
        MovingDirection = 3;                            //�����
        printf("%d\r\n",MotorCouter1);
        if(MotorCouter1 >3600 && MotorCouter2 > 3600)
        {break;}
    }

    MovingDirection = 1;

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
    //���ҩ��
    while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == SET);

    //��һ��   ��
    CarStart();

    //Ѳ��
    CarTracking();

    //����
    CarBackDistance(700);

    //�ڶ��� ��ת��  ��׷ƽ
    CarLeftTurn();

    //������ Ѳ�� ��ⲡ�����
    Res_Rback();    //�ȴ��ع�����
    HAL_Delay(100);
    //Ѳ��
    CarCallTracking();
   // CarTracking();
   // CarCallTracking();
  //  CarOver();



    CarStop();
    //�������
    HAL_GPIO_WritePin(Red_GPIO_Port,Red_Pin,RESET);

    //�ȴ�ж��
    while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

    //Ϩ����
    HAL_GPIO_WritePin(Red_GPIO_Port,Red_Pin,SET);

    //ԭ·����
    //����
    CarBackStraight();

    //�ȴ�һ������ �������෴��⣩   δд��
    CarBackTracking();

    //����һ�ξ���
    CarBackDistance(350);

    //�ڶ��� ��ת��
    CarLeftTurn();

    //��
    CarStart();
    Res_Rback();    //�ȴ��ع�����
    HAL_Delay(100);
    //Ѳ��
    CarCallTracking();
 //   CarTracking();
 //   CarOver();
    //ͣ
    CarStop();

    //�����̵�
    HAL_GPIO_WritePin(Green_GPIO_Port,Green_Pin,RESET);
    while(1);
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
    //���ҩ��
    while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == SET);

    //��һ��   ��
    CarStart();
    printf("STAR_OK");
    //Ѳ��
    CarTracking();
    printf("XUN_OK");
    //����
    CarBackDistance(700);

    //�ڶ��� ��ת��  ��׷ƽ
    CarRightTurn();
    printf("RF_OK");
    //������ Ѳ�� ��ⲡ�����
    //Ѳ��
    //CarOver();
    Res_Rback();    //�ȴ��ع�����
    HAL_Delay(100);
    //Ѳ��
    printf("ROOM_");
    //Ѳ��
    CarCallTracking();
    //CarTracking();
   // CarOver();
    CarStop();
    //�������
    HAL_GPIO_WritePin(Red_GPIO_Port,Red_Pin,RESET);

    //�ȴ�ж��
    while(HAL_GPIO_ReadPin(Medicine_GPIO_Port,Medicine_Pin) == RESET);

    //Ϩ����
    HAL_GPIO_WritePin(Red_GPIO_Port,Red_Pin,SET);
    printf("BACK_");
    //ԭ·����
    //����
    CarBackStraight();

    //�ȴ�һ������ �������෴��⣩
    CarBackTracking();

    //����һ�ξ���
    CarBackDistance(350);

    //�ڶ��� ��ת��
    CarRightTurn();

    //��
    CarStart();
    Res_Rback();    //�ȴ��ع�����

    HAL_Delay(100);
    //Ѳ��
    CarCallTracking();
    //CarTracking();
    //ͣ
    CarStop();

    //�����̵�
    HAL_GPIO_WritePin(Green_GPIO_Port,Green_Pin,RESET);
    while(1);
}


void BaseThree(void)      //������
{
    uint8_t  photo_y = 0;
    //��һ��
    //��
    CarStart();

    //Ѳ��  ��⵽��һ������
    CarTracking();

    HAL_Delay(500);

    //Ѳ��  ��⵽�ڶ�������
    CarTracking();
    //����
    CarBackDistance(1500);

    //����ͷ��� ���� ���ȷ��λ��

    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 65);//�������Ƕ�ֵ     ��
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 76);//�������Ƕ�ֵ     ��
    //�ж��Ƿ��� ���·�� ����1�� �ң�0�� ���ڣ�2��
    photo_y = Camera();
    if(photo_y == 1)       //�����·�� ���
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
        CarBackDistance(800);

        //�ڶ��� ��ת��
        CarRightTurn();

        //Ѳ��
        CarTracking();

        //Ѳ��
        CarTracking();

        //ͣ
        CarStop();
    }else if (photo_y == 0) //�����·�� �ұ�
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
        CarBackDistance(800);

        //�ڶ��� ��ת��
        CarRightTurn();

        //Ѳ��
        CarTracking();

        //Ѳ��
        CarTracking();

        //ͣ
        CarStop();
    } else if (photo_y == 2)
    {
        //��
        CarStart();

        //����
        CarBackDistance(800);

        //����ͷ��� ����


        //�ж������� ���� ���� (1)�� (0)��
        if (CameraTwo())    //Զ�˲��� ���
        {
            //��ת��
            CarLeftTurn();

            //ѭ��
            CarTracking();

            //����һ�ξ���
            CarBackDistance(800);

            //����ͷ��� ����

            //�ж������� ���� ���� (1)�� (0)��
            if(Camera())           //��Զ�˲��� ����� ���
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
                CarBackDistance(800);

                // ��ת��
                CarLeftTurn();

                //Ѳ��                !!!!!ֻ�а���� ��д���� ���������
                CarTracking();

                //����
                CarBackDistance(800);

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

            }else           //��Զ�˲��� ����� �ұ�
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
                CarBackDistance(800);

                // ��ת��
                CarRightTurn();

                //Ѳ��                !!!!!ֻ�а���� ��д���� ���������
                CarTracking();

                //����
                CarBackDistance(800);

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

        }else            //��Զ�˲��� ���Ҷ�
        {
            //��ת��
            CarRightTurn();

            //ѭ��
            CarTracking();

            //����һ�ξ���
            CarBackDistance(800);

            //����ͷ��� ����


            //�ж������� ���� ���� (1)�� (0)��
            if(Camera())           //��Զ�˲��� ���Ҷ� �����
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
                CarBackDistance(800);

                // ��ת��
                CarLeftTurn();

                //Ѳ��                !!!!!ֻ�а���� ��д���� ���������
                CarTracking();

                //����
                CarBackDistance(800);

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
                CarBackDistance(800);

                // ����
                CarRightTurn();

                //Ѳ��                !!!!!ֻ�а���� ��д���� ���������
                CarTracking();

                //����
                CarBackDistance(800);

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