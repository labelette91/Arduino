/**************************************************************************//**
 * \brief MAX7219 library for Arduino - Demo program
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
 * \file MAX7219.pde
 ******************************************************************************/

/******************************************************************************
 * Header file inclusions.
 ******************************************************************************/

#include <WProgram.h>

#include <SPI.h>

#include <MAX7219.h>

/******************************************************************************
 * Private variable declaration.
 ******************************************************************************/

static MAX7219 max7219(10);

/******************************************************************************
 * Public function definitions.
 ******************************************************************************/

/**************************************************************************//**
 * \fn void setup()
 ******************************************************************************/
void setup()
{
    // Initiliaze MAX7219.
    max7219.initialize();
    // Set scan limit.
    max7219.setScanLimit(MAX7219::ScanLimit_Digit0To7);
     // Set decode mode.
    max7219.setDecodeMode(MAX7219::DecodeMode_AllDigits);
    // Set intensity.
    max7219.setIntensity(MAX7219::Intensity_Level15);
    // Set shutdown mode.
    max7219.setShutdownMode(MAX7219::ShutdownMode_NormalOperation);
}

/**************************************************************************//**
 * \fn void loop()
 ******************************************************************************/
void loop()
{
    // To write digit values, a loop may be more elegant but a bit more
    // complicated for this example.

    // Write digit 0 value.
    max7219.writeDigit(MAX7219::Digit_0, MAX7219::Character_Zero);
    // Write digit 1 value.
    max7219.writeDigit(MAX7219::Digit_1, MAX7219::Character_One);
    // Write digit 2 value.
    max7219.writeDigit(MAX7219::Digit_2, MAX7219::Character_Two);
    // Write digit 3 value.
    max7219.writeDigit(MAX7219::Digit_3, MAX7219::Character_Three);
    // Write digit 4 value.
    max7219.writeDigit(MAX7219::Digit_4, MAX7219::Character_Four);
    // Write digit 5 value.
    max7219.writeDigit(MAX7219::Digit_5, MAX7219::Character_Five);
    // Write digit 6 value.
    max7219.writeDigit(MAX7219::Digit_6, MAX7219::Character_Six);
    // Write digit 7 value.
    max7219.writeDigit(MAX7219::Digit_7, MAX7219::Character_Seven);

    // Wait a little before next loop.
    delay(1000);
}
