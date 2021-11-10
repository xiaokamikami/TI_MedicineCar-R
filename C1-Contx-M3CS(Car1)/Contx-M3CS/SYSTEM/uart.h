#ifndef _UART_H_
#define _UART_H_

#define UART_1 1
#define UART_2 2
#define UART_3 3
#define UART_4 4
#define UART_5 5

extern char Uart_array_r[24];
extern char Uart_array_s[24];
extern unsigned char r_num,Read_Flag;

void UART_Init (char Channel);
char *  Num_Handle_Byte (char num);
void Clean(unsigned char Length,char *Opinter,char *dat);

#endif
