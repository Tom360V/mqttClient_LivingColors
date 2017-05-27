/*!
 *  \file    LivingColors.h
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
 *    Created: 03-10-2008
 */

#ifndef LIVINGCOLORS_H_
#define LIVINGCOLORS_H_

#include "CC2500.h"
#include "ColourConversion.h"
#include <inttypes.h>

#define MAXLAMPS 6

/*! \brief Class for interfacing with Philips LivingColors lamp.
 *
 * This class implements basic functions to communicate with the LivingColors lamp.
 */


/*!
 * Initializes communication with the LivingColors lamp.
 */
void LivingColors_init();

/*!
 * Returns the maximum number of storable lamps.
 *
 * \return Maximum number of lamps.
 */
unsigned char LivingColors_getMaxNumLamps();

/*!
 * Returns the number of currently known lamps.
 *
 * \return Number of known adresses.
 */
unsigned char LivingColors_getNumLamps();

/*!
 * Adds a lamp address to storage. Address must be 9 bytes long.
 *
 * \param[in] address Array holding lamp address.
 * \return Lamp index for address or -1 on failure.
 */
char LivingColors_addLamp(unsigned char* address);

/*!
 * Saves the address of lamp with given index in supplied array. Only fills array when index and
 * address are valid.
 *
 * \param[in] index Lamp index, must be 0 <= index < getNumLamps().
 * \param[in] address Array for lamp address, must be at least 9 bytes long.
 * \return True if array is filled with valid address, false otherwise.
 */
uint8_t LivingColors_getLamp(unsigned char index, unsigned char* address);

/*!
 * Tries to learn lamp addresses by snooping the commands which are sent by the official
 * remote. The learning time period is about 5 seconds; hold the official remote very close
 * to the board and press some commands (e.g. change colour, off, on).
 */
void LivingColors_learnLamps();

/*!
 * Clears all lamp addresses.
 */
void LivingColors_clearLamps();

/*!
 * Sets colour of lamp with given index. Updates colour only if lamp address is valid.
 *
 * \param[in] index Lamp index, must be 0 <= index < getNumLamps().
 * \param[in] r Red value for lamp (0-255).
 * \param[in] g Green value for lamp (0-255).
 * \param[in] b Blue value for lamp (0-255).
 */
void LivingColors_setLampColourRGB(unsigned char index, unsigned char r, unsigned char g, unsigned char b);

/*!
 * Sets colour of lamp with given index. Updates colour only if lamp address is valid.
 *
 * \param[in] index Lamp index, must be 0 <= index < getNumLamps().
 * \param[in] h Hue value for lamp (0-255).
 * \param[in] s Saturation value for lamp (0-255).
 * \param[in] v Value value for lamp (0-255).
 */
void LivingColors_setLampColourHSV(unsigned char index, unsigned char h, unsigned char s, unsigned char v);

/*!
 * Turns lamp with given index on and sets colour if lamp address is valid.
 *
 * \param[in] index Lamp index, must be 0 <= index < getNumLamps().
 * \param[in] r Red value for lamp (0-255).
 * \param[in] g Green value for lamp (0-255).
 * \param[in] b Blue value for lamp (0-255).
 */
void LivingColors_turnLampOnRGB(unsigned char index, unsigned char r, unsigned char g, unsigned char b);

/*!
 * Turns lamp with given index on and sets colour if lamp address is valid.
 *
 * \param[in] index Lamp index, must be 0 <= index < getNumLamps().
 * \param[in] h Hue value for lamp (0-255).
 * \param[in] s Saturation value for lamp (0-255).
 * \param[in] v Value value for lamp (0-255).
 */
void LivingColors_turnLampOnHSV(unsigned char index, unsigned char h, unsigned char s, unsigned char v);

/*!
 * Turns lamp with given index off if lamp address is valid.
 *
 * \param[in] index Lamp index, must be 0 <= index < getNumLamps();
 */
void LivingColors_turnLampOff(unsigned char index);

#endif
