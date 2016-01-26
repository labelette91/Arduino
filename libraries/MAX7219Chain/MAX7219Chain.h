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
 * \headerfile MAX7219.h
 ******************************************************************************/
 
#ifndef MAX7219_h
#define MAX7219_h

/******************************************************************************
 * Header file inclusions.
 ******************************************************************************/

#include <Arduino.h>

//number of digit per MAX7219
#define MAX7219_MAX_DIGIT (8)
/**************************************************************************//**
 * \class MAX7219
 ******************************************************************************/
class MAX7219
{
    public:
    
        /******************************************************************//**
         * \enum DecodeModes
         * \typedef DecodeMode_t
         **********************************************************************/
        typedef enum DecodeModes
        {
            DecodeMode_NoDecode     = 0x00,
            DecodeMode_Digit0       = 0x01,
            DecodeMode_Digit1       = 0x02,
            DecodeMode_Digit2       = 0x04,
            DecodeMode_Digit3       = 0x08,
            DecodeMode_Digit4       = 0x10,
            DecodeMode_Digit5       = 0x20,
            DecodeMode_Digit6       = 0x40,
            DecodeMode_Digit7       = 0x80,
            DecodeMode_AllDigits    = 0xFF
        } DecodeMode_t;

        /******************************************************************//**
         * \enum DecodeModes
         * \typedef DecodeMode_t
         **********************************************************************/
        typedef enum Intensities
        {
            Intensity_Level0    = 0x00,
            Intensity_Level1    = 0x01,
            Intensity_Level2    = 0x02,
            Intensity_Level3    = 0x03,
            Intensity_Level4    = 0x04,
            Intensity_Level5    = 0x05,
            Intensity_Level6    = 0x06,
            Intensity_Level7    = 0x07,
            Intensity_Level8    = 0x08,
            Intensity_Level9    = 0x09,
            Intensity_Level10   = 0x0A,
            Intensity_Level11   = 0x0B,
            Intensity_Level12   = 0x0C,
            Intensity_Level13   = 0x0D,
            Intensity_Level14   = 0x0E,
            Intensity_Level15   = 0x0F
        } Intensity_t;

        /******************************************************************//**
         * \enum ScanLimits
         * \typedef ScanLimit_t
         **********************************************************************/
        typedef enum ScanLimits
        {   
            ScanLimit_Digit0    = 0x00,
            ScanLimit_Digit0To1 = 0x01,
            ScanLimit_Digit0To2 = 0x02,
            ScanLimit_Digit0To3 = 0x03,
            ScanLimit_Digit0To4 = 0x04,
            ScanLimit_Digit0To5 = 0x05,
            ScanLimit_Digit0To6 = 0x06,
            ScanLimit_Digit0To7 = 0x07
        } ScanLimit_t;

        /******************************************************************//**
         * \enum ShutdownModes
         * \typedef ShutdownMode_t
         **********************************************************************/
        typedef enum ShutdownModes
        {
            ShutdownMode_Shutdown           = 0x00,
            ShutdownMode_NormalOperation    = 0x01
        } ShutdownMode_t;

        /******************************************************************//**
         * \enum DisplayTestModes
         * \typedef DisplayTestMode_t
         **********************************************************************/
        typedef enum DisplayTestModes
        {
            NormalOperation = 0x00,
            TestMode        = 0x01
        } DisplayTestMode_t;

        /******************************************************************//**
         * \enum Digits
         * \typedef Digit_t
         **********************************************************************/
        typedef enum Digits
        {
            Digit_0 = 0x01,
            Digit_1 = 0x02,
            Digit_2 = 0x03,
            Digit_3 = 0x04,
            Digit_4 = 0x05,
            Digit_5 = 0x06,
            Digit_6 = 0x07,
            Digit_7 = 0x08
        } Digit_t;

        /******************************************************************//**
         * \enum Characters
         * \typedef Character_t
         **********************************************************************/
        typedef enum Characters
        {
            Character_Zero  = 0x00,
            Character_One   = 0x01,
            Character_Two   = 0x02,
            Character_Three = 0x03,
            Character_Four  = 0x04,
            Character_Five  = 0x05,
            Character_Six   = 0x06,
            Character_Seven = 0x07,
            Character_Eight = 0x08,
            Character_Nine  = 0x09,
            Character_Dash  = 0x0A,
            Character_E     = 0x0B,
            Character_H     = 0x0C,
            Character_L     = 0x0D,
            Character_P     = 0x0E,
            Character_Blank = 0x0F
        } Character_t;

        /******************************************************************//**
         * \enum Segments
         * \typedef Segment_t
         **********************************************************************/
        typedef enum Segments
        {
            Segment_DP  = 0x80,
            Segment_A   = 0x40,
            Segment_B   = 0x20,
            Segment_C   = 0x10,
            Segment_D   = 0x08,
            Segment_E   = 0x04,
            Segment_F   = 0x02,
            Segment_G   = 0x01
        } Segment_t;

    public:
    
        /******************************************************************//**
         * \fn MAX7219(byte csPin)
         *
         * \brief Constructor.
         *
         * \param	csPin	Chip select pin number.
         **********************************************************************/
        MAX7219
        (
            byte csPin,
            byte DeviceNumber = 1
        );

        /******************************************************************//**
         * \fn void initialize()
         *
         * \brief Initialize SPI to drive MAX7219.
         **********************************************************************/
        void
        initialize();     

        /******************************************************************//**
         * \fn void setDecodeMode(DecodeModes mode)
         *
         * \brief Set MAX7219 decode mode.
         *
         * \param	mode	Decode mode to set.
         **********************************************************************/
        void
        setDecodeMode
        (
            DecodeModes mode
            ,byte deviceNumber = 0
        );

        /******************************************************************//**
         * \fn void setIntensity(Intensities intensity)
         *
         * \brief Set MAX7219 intensity.
         *
         * \param	itensity	Intensity to set.
         **********************************************************************/
        void
        setIntensity
        (
            Intensities intensity
            ,byte deviceNumber = 0
        );

        /******************************************************************//**
         * \fn void setScanLimit(ScanLimits limit)
         *
         * \brief Set MAX7219 scan limit.
         *
         * \param	limit	Scan limit to set.
         **********************************************************************/
        void
        setScanLimit
        (
            ScanLimits limit
            ,byte deviceNumber = 0
        );

        /******************************************************************//**
         * \fn void setShutdownMode(ShutdownModes mode)
         *
         * \brief Set MAX7219 shutdown mode.
         *
         * \param	mode	Shutdown mode to set.
         **********************************************************************/
        void
        setShutdownMode
        (
            ShutdownModes mode
            ,byte deviceNumber = 0
        );

        /******************************************************************//**
         * \fn void setDisplayTestMode(DisplayTestModes mode)
         *
         * \brief Set MAX7219 display test mode.
         *
         * \param	mode	Display test mode to set.
         **********************************************************************/
        void
        setDisplayTestMode
        (
            DisplayTestModes mode
            ,byte deviceNumber = 0
        );

        /******************************************************************//**
         * \fn void writeDigit(
         * Digits       digit,
         * Characters   character,
         * bool         decimalPoint = false)
         *
         * \brief Write character on digit.
         *
         * \param	digit		    Digit to write.
         * \param	character	    Character to write.
         * \param   decimalPoint    Display decimal point.
         **********************************************************************/
        void
        writeDigit
        (
            byte      digit,
            byte  character,
            bool        decimalPoint = false
            ,byte deviceNumber = 0
        );
				void
				writeDigits
				(
				    byte      digit,
				    byte  character,
				    bool        decimalPoint = false
				);
        
    private:
    
        byte m_csPin;
        byte m_DeviceNumber; //number of chained devices

        /******************************************************************//**
         * \fn void write(byte address, byte value)
         *
         * \brief Write value into MAX7219 register.
         *
         * \param	address	Register address.
         * \param	value	Value to write.
         **********************************************************************/
        void
        write
        (
            byte address,
            byte value
            ,byte deviceNumber = 0
        );
};

#endif // MAX7219_h
