#include "stm32f10x.h"
#include "main.h"

#include "usart.h"

#include "cJson.h"
#include "string.h"
#include "delay.h"
#include "stdlib.h"
extern __IO u8 sys_mode;
//static volatile	char *commonend;   //指令存放

void uart_init(u32 bound);
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);
u8 DMA_Rece_Buf[100];
u8 Print_Buf[50];
static void Json_RX(char * out);
char *Json_ptr;		  //Json字符

__IO int k2_num = 0;   //获取图像
__IO int k2_tag_x  = 0;   //指示目标
//__IO int k2_val = 0;   //图像大小拟合距离 
__IO u8 BALL_RX_FLAG = 0;

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

};

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0){}//循环发送,直到发送完毕   
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 

//函数名：  Json_Extract
//作者：    Kamimiaomiao
//日期：    2021-6-13
//功能：    提取Json格式数据
//输入参数：AT透传接收的数据      
//返回值：  透传接收到的Json数据
//修改记录：
//使用方法：传入透传的数据返回透传流内Json格式部分的字符串	   

static char* Json_Extract(char * Json_ptr ,char * Rxb)			//提取Json指令
{
	u8 i = 0;
	Json_ptr = strchr(Rxb,'{');
	for(i=0;i<sizeof(Json_ptr);i++)
	{
		if(Json_ptr[i] == '}')
		{
			Json_ptr[i+1] = 0;
			break;
		}
	}
	
	return  Json_ptr;
}

//函数名：  Json_RX
//作者：    Kamimiaomiao
//日期：    2021-6-13
//功能：    解析返回的Json数据
//输入参数：Json格式数据      
//返回值： //会直接存新指令到对应的变量里 

//修改记录：
//使用方法：在初始化指针添加你的json_参数名字	   
//      	然后json_参数名字 = cJSON_GetObjectItem( json ,"参数名字");
//          获得命令后按自己要求处理 默认是显示在屏幕上
static void Json_RX(char * out)			//解析Json指令
{
    cJSON *json,*json_num,*json_tag_x;
    //out="{\"one\":\"long\",\"two\":\"2\",\"three\":3}"; //JSON格式示例
	
//	char commend1[10];
//	commonend = commend1;
	
    json = cJSON_Parse(out); //将得到的字符串解析成json形式
    json_num  = cJSON_GetObjectItem( json ,"n");  //从json获取键值内容
	json_tag_x  = cJSON_GetObjectItem( json ,"x");//从json获取键值内容
//	json_va = cJSON_GetObjectItem( json ,"v");//从json获取键值内容
//  sprintf((char *)TXbuf,"\r\n one:%s   two:%lf   three:%d",json_one->valuestring,json_two->valuedouble,json_three->valueint);
//**解析法1
//	strcpy((char *)commonend, json_x->valuestring);
//	k2_x  = atoi((char *)commonend);
//	strcpy((char *)commonend, json_y->valuestring);
//	k2_y  = atoi((char *)commonend);
//	strcpy((char *)commonend, json_co->valuestring);
//	k2_color  = atoi((char *)commonend);	
//	printf("comm:%s\n",commonend);
	
//**解析法2
	k2_num = json_num->valueint;	
	k2_tag_x = json_tag_x->valueint;	
//	k2_val = json_va->valueint;
	//printf("k2:%d",k2_x);



//	sprintf((char *)TXbuf,"MQTT:command:%s \r\n ",json_command->valuestring);	//串口返回指令		
//	HAL_UART_Transmit(&huart1, (uint8_t *)TXbuf, sizeof(TXbuf),200);
//	memset((char *)TXbuf, 0, sizeof(TXbuf)); 
	
	delay_ms(10);
    cJSON_Delete(json);  //释放内存 
//	return json_command->valuestring;
} 

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

   RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_GPIOA,ENABLE); //使能USART1，GPIOA时钟
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //使能DMA传输
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟

   USART_DeInit(USART2);  //复位串口1
   //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置
  USART_InitStructure.USART_BaudRate = bound;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;//无硬件数据流控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//开启空闲中断
    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);   //使能串口1 DMA接收
    USART_Cmd(USART2, ENABLE);                   //使能串口 
 
    //相应的DMA配置
  DMA_DeInit(DMA1_Channel6);   //将DMA的通道5寄存器重设为缺省值  串口1对应的是DMA通道5
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR; //DMA外设usart基地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)DMA_Rece_Buf;  //DMA内存基地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
  DMA_InitStructure.DMA_BufferSize = DMA_Rec_Len;  //DMA通道的DMA缓存的大小
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //数据宽度为8位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道

    DMA_Cmd(DMA1_Channel6, ENABLE);  //正式驱动DMA传输
}

//重新恢复DMA指针
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //关闭USART1 TX DMA1所指示的通道    
 	DMA_SetCurrDataCounter(DMA_CHx,DMA_Rec_Len);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_CHx, ENABLE);  //打开USART1 TX DMA1所指示的通道  
}	

//发送len个字节
//buf:发送区首地址
//len:发送的字节数
void Usart2_Send(u8 *buf,u8 len)
{
	u8 t;
  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART2,buf[t]);
	}	 
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		
}

//串口中断函数
void USART2_IRQHandler(void)                //串口1中断服务程序
{
	static u16 Usart2_Rec_Cnt;
     if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
      {
          USART_ReceiveData(USART2);//读取数据注意：这句必须要，否则不能够清除中断标志位。
		  
			if(BALL_RX_FLAG == 0)
			{
				Usart2_Rec_Cnt =DMA_Rec_Len-DMA_GetCurrDataCounter(DMA1_Channel6); //算出接本帧数据长度
	   
			 //***********帧数据处理函数************//
				Json_RX((char * )DMA_Rece_Buf) ;
				//printf("x:%d,y:%d,color:%d \n",k2_x,k2_y,k2_color);
				BALL_RX_FLAG = 1;		
			  }

        //*************************************//
         USART_ClearITPendingBit(USART2,USART_IT_IDLE);         //清除中断标志
         MYDMA_Enable(DMA1_Channel6);                  //恢复DMA指针，等待下一次的接收
     } 

}



