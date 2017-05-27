/*
 * uart.h
 *
 *  Created on: Oct 17, 2015
 *      Author: tom
 */

#ifndef UART_H_
#define UART_H_

#include <stddef.h>
//#include "communication_interface.h"

//extern driver_fpHandles_t UART_Handles;
//!!DELETE
typedef void (*fpReceive_t)(uint8_t *data, uint8_t n, int16_t handle);
typedef struct
{
    fpReceive_t fpReceive;
}commEngine_fpHandles_t;
//!!DELETE

int8_t  UART_Init(char *pPortname, commEngine_fpHandles_t *pfpHandles);
int8_t  UART_Flush();
int8_t  UART_WriteStr(uint8_t *str, uint8_t length, int16_t handle);
char    UART_Task();

void   UART_Debug();

#endif /* UART_H_ */
