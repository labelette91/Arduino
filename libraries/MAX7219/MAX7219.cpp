/**************************************************************************//**
 * \brief MAX7219 library for Arduino
 * \author Copyright (C) 2011  Julien Le Sech - www.idreammicro.com
 * \version 1.0
 * \date 20110801
 *
 * This file is part of the MAX7219 library for Arduino.
 *
 * This library is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 * 
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/
 ******************************************************************************/

/**************************************************************************//**
 * \file MAX7219.cpp
 ******************************************************************************/
 
/******************************************************************************
 * Header file inclusions.
 ******************************************************************************/
 
#include <WProgram.h>

#include <SPI.h>

#include <MAX7219.h>

/******************************************************************************
 * Private macros.
 ******************************************************************************/

#define REG_NO_OP           0x00
#define REG_DIGIT_0         0x01
#define REG_DIGIT_1         0x02
#define REG_DIGIT_2         0x03
#define REG_DIGIT_3         0x04
#define REG_DIGIT_4         0x05
#define REG_DIGIT_5         0x06
#define REG_DIGIT_6         0x07
#define REG_DIGIT_7         0x08
#define REG_DECODE_MODE     0x09
#define REG_INTENSITY       0x0A
#define REG_SCAN_LIMIT      0x0B
#define REG_SHUTDOWN        0x0C
#define REG_DISPLAY_TEST    0x0F

/******************************************************************************
 * Public method definitions.
 ******************************************************************************/

/**************************************************************************//**
 * \fn MAX7219::MAX7219(byte csPin)
 *
 * \brief Constructor.
 *
 * \param	csPin	Chip select pin number.
 ******************************************************************************/
MAX7219::MAX7219
(
    byte csPin
){
	m_csPin = csPin;
}

/**************************************************************************//**
 * \fn void MAX7219::initialize()
 *
 * \brief Initialize SPI to drive MAX7219.
 ******************************************************************************/
void
MAX7219::initialize()
{
    // Configure chip select pin as output.
    pinMode(m_csPin, OUTPUT);
    
    // Wait a little to allow MAX7219 to see a correct logic level on CS pin.
    delay(1);
    
    // Configure SPI.
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV8);
}

/**************************************************************************//**
 * \fn void MAX7219::setDecodeMode(DecodeModes mode)
 *
 * \brief Set MAX7219 decode mode.
 *
 * \param	mode	Decode mode to set.
 ******************************************************************************/
void
MAX7219::setDecodeMode
(
    DecodeModes mode
){
    write(REG_DECODE_MODE, mode);
}

/**************************************************************************//**
 * \fn void MAX7219::setIntensity(Intensities intensity)
 *
 * \brief Set MAX7219 intensity.
 *
 * \param	itensity	Intensity to set.
 ******************************************************************************/
void
MAX7219::setIntensity
(
    Intensities intensity
){
    write(REG_INTENSITY, intensity);
}

/**************************************************************************//**
 * \fn void MAX7219::setScanLimit(ScanLimits limit)
 *
 * \brief Set MAX7219 scan limit.
 *
 * \param	limit	Scan limit to set.
 ******************************************************************************/
void
MAX7219::setScanLimit
(
    ScanLimits limit
){
    write(REG_SCAN_LIMIT, limit);
}

/**************************************************************************//**
 * \fn void MAX7219::setShutdownMode(ShutdownModes mode)
 *
 * \brief Set MAX7219 shutdown mode.
 *
 * \param	mode	Shutdown mode to set.
 ******************************************************************************/
void
MAX7219::setShutdownMode
(
    ShutdownModes mode
){
    write(REG_SHUTDOWN, mode);
}

/**************************************************************************//**
 * \fn void MAX7219::setDisplayTestMode(DisplayTestModes mode)
 *
 * \brief Set MAX7219 display test mode.
 *
 * \param	mode	Display test mode to set.
 ******************************************************************************/
void
MAX7219::setDisplayTestMode
(
    DisplayTestModes mode
){
    write(REG_DISPLAY_TEST, mode);
}

/**************************************************************************//**
 * \fn void MAX7219::writeDigit(
 * Digits       digit,
 * Characters   character,
 * bool         decimalPoint)
 *
 * \brief Write character on digit.
 *
 * \param	digit		    Digit to write.
 * \param	character	    Character to write.
 * \param   decimalPoint    Display decimal point if true.
 ******************************************************************************/
void
MAX7219::writeDigit
(
    Digits      digit,
    Characters  character,
    bool        decimalPoint
){
    byte value = character;
    if (decimalPoint)
    {
        // If decimal point must be switched on.
        value |= Segment_DP;
    }
    write(digit, value);
}

/**************************************************************************//**
 * \fn void MAX7219::writeDigit(Digits digit, Segments segments)
 *
 * \brief Set segment(s) on digit.
 *
 * \param	digit	Digit to write.
 * \param	segment	Segment(s) to set.
 ******************************************************************************/
void
MAX7219::writeSegment
(
    Digits      digit,
    Segments    segments
){
    write(digit, segments);
}

/******************************************************************************
 * Private method definitions.
 ******************************************************************************/

/**************************************************************************//**
 * \fn void MAX7219::write(byte address, byte value)
 *
 * \brief Write value into MAX7219 register.
 *
 * \param	address	Register address.
 * \param	value	Value to write.
 ******************************************************************************/
void
MAX7219::write
(
    byte address,
    byte value
){
    // Reset chip select pin to select the device.
    digitalWrite(m_csPin, LOW);
    // Transmit data to the device, register address and value.
    SPI.transfer(address);
    SPI.transfer(value);
    // Set chip select pin to valid data onto the device.
    digitalWrite(m_csPin, HIGH);
}
