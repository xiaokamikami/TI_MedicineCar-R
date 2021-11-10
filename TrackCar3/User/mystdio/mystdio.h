#ifndef __MYSTDIO_H
#define __MYSTDIO_H

#include "stm32f1xx_hal.h"
#include <sys/stat.h>
#include <stdio.h>

void mystdioInit(UART_HandleTypeDef *huart);
void RetargetInit(UART_HandleTypeDef *huart);
int _isatty(int fd);
int _write(int fd, char *ptr, int len);
int _close(int fd);
int _lseek(int fd, int ptr, int dir);
int _read(int fd, char *ptr, int len);
int _fstat(int fd, struct stat *st);

#endif

