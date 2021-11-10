#ifndef _SPI__H_

#define _SPI__H_

/* SPI Software 		2020.08.22*/
/* SPI Optimizes 		2020.08.24*/
/* SPI FINISH   	    2020.7.7  */
/* SPI Stable version   2020.8.27 */

/* SPI Master-Slave C51	2020.8.27 */
/* SPI C51 不允许在主函数调用相关引脚   */

extern char SPI_array_s[20];

void SPI_Software_Init(char num);                        //软件初始化
void SPI_SetSpeed(unsigned int SPI_BaudRatePrescaler_x); //速度分频

//char SPI_Read_And_Write_Byte(char num, char dat); //Slave
//char *SPI_Read_And_Write(char num, const char *array);

char SPI_Write_And_Read_Byte(char num, char dat); //Master
char *SPI_Write_And_Read(char num, const char *array, char Len); //通道选择 ARRAY

#endif
