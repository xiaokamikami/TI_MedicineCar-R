#include "OpenmvData.h"

int Xdate,Ydate;	
char xx[5],yy[5];
int TempXdate,TempYdate;


///**
//  * @brief  Openmv数据处理 把原始数据[xxx,yyy]，仅留x,y的数据
//  * @param  全局变量 Xdate,Ydate X轴的数据 Y轴的数据
//  * @retval 无
//  */
//int OpenmvData(void)
//{
//    printf("--------3-------\n");
//
//    int x=0,y=0,z=0,t=0;
//    if(UART2_Rx_flg)//接收完成标志（判断是否接收完数据）
//    {
//        printf("--------2-------\n");
//        for (t = 0; t < UART2_Rx_cnt; t++)//通过Uart2接收数据计数器来确定数据所处的位
//        {
//            /* 原始数据为：[xxx,yyy] 此段为去除‘[’‘,’‘]’三个符号，仅留x,y的数据*/
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
//        Xdate = atoi(&xx[0]);//(atoi():是一个把字符串转换为整型的函数)
//        Ydate = atoi(&yy[0]);
//
//
//        printf("X=%d\n", Xdate);//以Uart1为通道通过重定向函数printf()打印x,y数据
//        printf("Y=%d\n", Ydate);
//

//    }
//    HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);
//    UART2_Rx_cnt = 0;
//    UART2_Rx_flg = 0;
//    return 0;
//}



/**
  * @brief  Openmv数据处理 把原始数据[xxx,yyy]，仅留x,y的数据
  * @param  全局变量 Xdate,Ydate X轴的数据 Y轴的数据
  * @retval 无
  */
int OpenmvData(void)
{
    int x=0,y=0,z=0,t=0;
    if(UART2_Rx_flg)//接收完成标志（判断是否接收完数据）
    {
        printf("--------2-------\n");
        for (t = 0; t < UART2_Rx_cnt; t++)//通过Uart2接收数据计数器来确定数据所处的位
        {
            /* 原始数据为：[xxx,yyy] 此段为去除‘[’‘,’‘]’三个符号，仅留x,y的数据*/
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

        Xdate = atoi(&xx[0]);//(atoi():是一个把字符串转换为整型的函数)
        Ydate = atoi(&yy[0]);
        printf("X=%d\n", Xdate);//以Uart1为通道通过重定向函数printf()打印x,y数据
        printf("Y=%d\n", Ydate);

        if (Xdate == 1)
        {
            /* 初始化 接收数据计数器,接收完成标志*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 2)
        {
            /* 初始化 接收数据计数器,接收完成标志*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 3)
        {
            /* 初始化 接收数据计数器,接收完成标志*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 4)
        {
            /* 初始化 接收数据计数器,接收完成标志*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 5)
        {
            /* 初始化 接收数据计数器,接收完成标志*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 6)
        {
            /* 初始化 接收数据计数器,接收完成标志*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 7)
        {
            /* 初始化 接收数据计数器,接收完成标志*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        } else if (Xdate == 8)
        {
            /* 初始化 接收数据计数器,接收完成标志*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;

            return 1;
        }
        else
        {
            /* 初始化 接收数据计数器,接收完成标志*/
            UART2_Rx_cnt = 0;
            UART2_Rx_flg = 0;
            HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);
            return 0;

        }
    }
    HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);

    return 0;
}