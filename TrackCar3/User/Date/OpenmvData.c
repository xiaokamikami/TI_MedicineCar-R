#include "OpenmvData.h"

int Xdate,Ydate;	
char xx[5],yy[5];
int TempXdate,TempYdate;


///**
//  * @brief  Openmv���ݴ��� ��ԭʼ����[xxx,yyy]������x,y������
//  * @param  ȫ�ֱ��� Xdate,Ydate X������� Y�������
//  * @retval ��
//  */
//int OpenmvData(void)
//{
//    printf("--------3-------\n");
//
//    int x=0,y=0,z=0,t=0;
//    if(UART2_Rx_flg)//������ɱ�־���ж��Ƿ���������ݣ�
//    {
//        printf("--------2-------\n");
//        for (t = 0; t < UART2_Rx_cnt; t++)//ͨ��Uart2�������ݼ�������ȷ������������λ
//        {
//            /* ԭʼ����Ϊ��[xxx,yyy] �˶�Ϊȥ����[����,����]���������ţ�����x,y������*/
//            if (UART2_Rx_Buf[t] == '[')x = 1, y = 0;
//            if (UART2_Rx_Buf[t] == ',') {
//                x = 0, y = 1;
//                xx[t - 1] = '\0';
//            }
//            if (UART2_Rx_Buf[t] == ']') {
//                yy[z] = '\0';
//                x = 0, y = 0, z = 0;
//            }
//            if (x == 1 && UART2_Rx_Buf[t] != '[')xx[t - 1] = UART2_Rx_Buf[t];
//            if (y == 1 && UART2_Rx_Buf[t] != ',') {
//                yy[z] = UART2_Rx_Buf[t];
//                z++;
//            }
//        }
//
//        Xdate = atoi(&xx[0]);//(atoi():��һ�����ַ���ת��Ϊ���͵ĺ���)
//        Ydate = atoi(&yy[0]);
//
//
//        printf("X=%d\n", Xdate);//��Uart1Ϊͨ��ͨ���ض�����printf()��ӡx,y����
//        printf("Y=%d\n", Ydate);
//

//    }
//    HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);
//    UART2_Rx_cnt = 0;
//    UART2_Rx_flg = 0;
//    return 0;
//}



/**
  * @brief  Openmv���ݴ��� ��ԭʼ����[xxx,yyy]������x,y������
  * @param  ȫ�ֱ��� Xdate,Ydate X������� Y�������
  * @retval ��
  */
int OpenmvData(void)
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

        Xdate = atoi(&xx[0]);//(atoi():��һ�����ַ���ת��Ϊ���͵ĺ���)
        Ydate = atoi(&yy[0]);
        printf("X=%d\n", Xdate);//��Uart1Ϊͨ��ͨ���ض�����printf()��ӡx,y����
        printf("Y=%d\n", Ydate);

        if (Xdate == 1)
        {
            /* ��ʼ�� �������ݼ�����,������ɱ�־*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 2)
        {
            /* ��ʼ�� �������ݼ�����,������ɱ�־*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 3)
        {
            /* ��ʼ�� �������ݼ�����,������ɱ�־*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 4)
        {
            /* ��ʼ�� �������ݼ�����,������ɱ�־*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 5)
        {
            /* ��ʼ�� �������ݼ�����,������ɱ�־*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 6)
        {
            /* ��ʼ�� �������ݼ�����,������ɱ�־*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 7)
        {
            /* ��ʼ�� �������ݼ�����,������ɱ�־*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 8)
        {
            /* ��ʼ�� �������ݼ�����,������ɱ�־*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        }
        else
        {
            /* ��ʼ�� �������ݼ�����,������ɱ�־*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;
            HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);
            return 0;

        }
    }
    HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);

    return 0;
}