/*
 * uart.c
 *
 *  Created on: Oct 17, 2015
 *      Author: tom
 */
#include "avr/io.h"
#include "avr/interrupt.h"
#include "uart.h"

#if 0
  #if defined (__AVR_ATmega328P__)
    #if define (UBRR0H)
    #endif
    #if define (UBRRH)
    #endif
  #endif
#endif

/******************************************************************************
 * DEFINES
 *****************************************************************************/
#define FOSC 16000000 // Clock Speed
#define BAUD 19200
#define MYUBRR FOSC/16/BAUD

const char cLUT[16]="0123456789ABCDEF";

commEngine_fpHandles_t *fpUartHandles;
/******************************************************************************
 * PRIVATE PROTOTYPE
 *****************************************************************************/
static void UART_Write(uint8_t data);

/******************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/

ISR(USART_RX_vect)
{
    uint8_t newChar = UDR0;
    if( 0 != fpUartHandles->fpReceive)
    {
        (fpUartHandles->fpReceive)(&newChar, 1, 0);
    }
}

static void UART_Write(uint8_t data)
{
    /* Wait for empty transmit buffer */
    while( !(UCSR0A & (1<<UDRE0)))
    {
        // Do nothing
    }

    // Send data!
    UDR0 = data;
}

/******************************************************************************
 * PUBLIC FUNCTIONS
 *****************************************************************************/
int8_t UART_Init(char *pPortname, commEngine_fpHandles_t *pfpHandles)
{
    (void)pPortname;
    fpUartHandles = pfpHandles;

    unsigned int ubrr = MYUBRR;

    // Set baud rate
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;

    UCSR0A = 0x00;
    // Double  the USART transmission speed
//    UCSR0A = (1<<U2X0);

    // Enable receiver and transmitter,
    UCSR0B  = (1<<RXEN0) | (1<<TXEN0);
    //Enable Receive Complete Interrupt
    UCSR0B |= (1<<RXCIE0);

    // Set frame format:
    UCSR0C  = (0<<USBS0);    // 1stop bit, No parity, Asynchronous
    UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);   // 8data bits

    return 0;
}

int8_t UART_Flush()
{
    while ( UCSR0A & (1<<RXC0) )
    {
        unsigned char dummy = UDR0;
    }        

    return 0;    
}

int8_t UART_WriteStr(uint8_t *str, uint8_t length, int16_t handle)
{
    (void)handle;
    uint8_t teller=0;

    while(teller < length)
    {
        UART_Write(str[teller]);
        teller++;
    }
    return teller;
}

void UART_WriteIntAsDecimal(unsigned int uiData)
{
    char cTemp;
    cTemp = cLUT[uiData/100];
    UART_Write(cTemp);

    uiData = uiData%100;
    cTemp = cLUT[uiData/10];
    UART_Write(cTemp);

    uiData = uiData%10;
    cTemp = cLUT[uiData%10];
    UART_Write(cTemp);
}

char UART_Task()
{
    return 0;
}
