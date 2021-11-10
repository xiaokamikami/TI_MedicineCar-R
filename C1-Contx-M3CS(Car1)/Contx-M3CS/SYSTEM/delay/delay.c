#include "delay.h"
#include "stm32f10x.h"
 
double  fac_us=0;								   						
static RCC_ClocksTypeDef  RCC_Clocks;	
void delay_init()
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8 );	//64M-->1us:64
	RCC_GetClocksFreq(&RCC_Clocks);
	fac_us=RCC_Clocks.SYSCLK_Frequency/8000000;//fac_us=64;		 
}								    
    								   
void delay_us(unsigned int nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;					  	 
	ticks=nus*fac_us; 					 		 
	tcnt=0;				
	told=SysTick->VAL;        				
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;		
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;
		}  
	};								    
}
 
void delay_ms(unsigned short nms)
{		 						   
	delay_us((u32)(nms*1000));	
}


