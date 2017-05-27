/*!
 *  \file    CC2500.cpp
 *  \version 1.1
 *  \date    29-01-2009
 *  \author  George Mathijssen, george.knutsel@gmail.com
 *
 *  Copyright (c) 2008, 2009 George Mathijssen
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  \internal
 *    Created: 02-10-2008
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "CC2500.h"

#define INPUT	0
#define OUTPUT  1

#define HIGH    0x01

#define SET_DDR(PORT, PIN, DIR)		{PORT |= (DIR<<PIN);}

#define SET_PIN(PORT, PIN)	PORT |=  (1<<PIN)
#define CLR_PIN(PORT, PIN)	PORT &= ~(1<<PIN)

#define READ_PIN(PORT, PIN)	((PORT>>PIN)&0x01)

#define PIN_CS		2       /* PB2 */
#define PIN_MOSI	3       /* PB3 */
#define PIN_MISO	4       /* PB4 */
#define PIN_SCK		5       /* PB5 */

void CC2500_init()
{
    // setup pin mode
	SET_DDR(DDRB, PIN_CS,   OUTPUT);
	SET_DDR(DDRB, PIN_MOSI, OUTPUT);
	SET_DDR(DDRB, PIN_MISO, INPUT);
	SET_DDR(DDRB, PIN_SCK,  OUTPUT);

    // disable device
	SET_PIN(PORTB, PIN_CS);

    // setup SPI control register: SPCR = 01010000
    // interrupt disabled, spi enabled, msb 1st, master, clk low when idle,
    // sample on rising edge of clk, system clock rate fastest
    SPCR = (1<<SPE) | (1<<MSTR);
    
    // clear data registers
    uint8_t clr = SPSR;
    clr = SPDR;
}


void delayMicroseconds( uint32_t us )
{
    while (us--)
    {
        __asm__(" NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t");
    };
}

void CC2500_reset()
{
    // enable device
	CLR_PIN(PORTB, PIN_CS);

    delayMicroseconds(1);

    // disable device and wait at least 40 microseconds
	SET_PIN(PORTB, PIN_CS);
    delayMicroseconds(41);

    // enable device
	CLR_PIN(PORTB, PIN_CS);

    // wait for device
    while (READ_PIN(PINB, PIN_MISO) == HIGH);

    // send reset command (SRES)
    CC2500_spiTransfer(0x30);

    // disable device
	SET_PIN(PORTB, PIN_CS);
}

unsigned char CC2500_sendByte(unsigned char data)
{
    // enable device
	CLR_PIN(PORTB, PIN_CS);

    // wait for device
    while (READ_PIN(PINB, PIN_MISO) == HIGH);

    // send byte
    unsigned char result = CC2500_spiTransfer(data);

    // disable device
 	SET_PIN(PORTB, PIN_CS);

    // return result
    return result;
}

unsigned char CC2500_sendCommand(unsigned char command, unsigned char data)
{
    // enable device
	CLR_PIN(PORTB, PIN_CS);

    // wait for device
    while (READ_PIN(PINB, PIN_MISO) == HIGH);

    // send command byte
    CC2500_spiTransfer(command);

    // send data byte
    unsigned char result = CC2500_spiTransfer(data);

    // disable device
	SET_PIN(PORTB, PIN_CS);

    // return result
    return result;
}

unsigned char CC2500_sendStrobeCommand(unsigned char command)
{
    // send command
    return CC2500_sendByte(command);
}

unsigned char CC2500_sendBurstCommand(unsigned char command, unsigned char* data,
    unsigned char length)
{
    // enable device
	CLR_PIN(PORTB, PIN_CS);

    // wait for device
    while (READ_PIN(PINB, PIN_MISO) == HIGH);

    // send command byte
    CC2500_spiTransfer(command);

    unsigned char result = 0;

    // send data bytes
    for (int i=0; i<length; ++i) {
        result = CC2500_spiTransfer(data[i]);
        data[i] = result;
    }

    // disable device
	SET_PIN(PORTB, PIN_CS);

    // return result
    return result;
}

unsigned char CC2500_spiTransfer(volatile unsigned char data)
{
    // start transmission
    SPDR = data;

    // wait for end of transmission
    while (!(SPSR & (1<<SPIF)));

    // return received byte
    return SPDR;
}
