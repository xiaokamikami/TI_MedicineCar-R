#ifndef __OLED_S__H_

#define __OLED_S__H_		  


/* 			OLED 0.96 SPI			 */
/* 			主程序无权调用 相关GPIO			 */ 


#define SIZE 16				// 12 / 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	 


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
#define OLED_MODE 0

//OLED控制用函数

void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);
void OLED_Fill(char x1,char y1,char x2,char y2,char dot);

void OLED_Init(void);

void OLED_ShowChar(char x,char y,char chr);
void OLED_ShowNum(char x,char y,unsigned int num,char len,char size);

void OLED_ShowString(char x,char y,const char *p);	 
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);

#endif
