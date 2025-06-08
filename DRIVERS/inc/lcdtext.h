/**
* @file		lcdtext.h
* @brief	Contains the text LCD API.
* @version	1.0
* @date		30 Out 2018
* @author	PSampaio
*
* Copyright(C) 2015-2023, PSampaio
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
**********************************************************************/

#ifndef LCDTEXT_H_
#define LCDTEXT_H_

/** @addtogroup DRIVERS
 * @{
 */

/** @defgroup LCD Text LCD
 * @brief This package provides the interface for driving 4-bit HD44780-based LCDs used
 * in LPCXpresso development board based on LPC1769 from NXP.
 *
 * The LCD data bits are connect to the microcontroller LPC1769 as show in the follow table:
 *  LCD | LPC1769
 * :--------: | :-----------:
 * D0 .. D3 | Not connected
 * D4 .. D7 | P2.0 .. P2.3
 * EN | P0.10
 * RS | P0.11
 * WR | GND
 *
 * @{
 */

#define LCDText_LINES		2	/*!< LCD number of lines   */
#define LCDText_COLUMNS		16	/*!< LCD number of columns */

/**
 * Initializes the LCD API.
 *
 * @return	None.
 *
 * @note	This function must be called prior to any other LCDText functions.
 */
void LCDText_Init();

/**
 * Write a character to the LCD
 *
 * @param c The character to write to the display
 */
void LCDText_WriteChar(char c);

/**
 * Write a C-string to the LCD
 *
 * @param str The C-string to write to the display
 */
void LCDText_WriteString(const char *str);

/**
 * Write a C-string to specified line of the LCD
 *
 * @param firstLine The C-string to write to the first of display. If NULL nothing is write.
 * @param secondLine The C-string to write to the second of display. If NULL nothing is write.
 *
 * @return None.
 */
void LCDText_WriteLine(const char *firstLine, const char *secondLine);

/**
 * Clear the screen and locate cursor to home position (0,0)
 *
 * @return None.
 */
void LCDText_Clear();

/**
 * Locate cursor to a screen line and column
 *
 * @param line    The vertical position from the top, indexed from 0
 * @param column  The horizontal position from the left, indexed from 0
 *
 * @return None.
 */
void LCDText_Locate(int line, int column);

/**
 * Turns cursor on.
 *
 * @return None.
 */
void LCDText_CursorOn(void);

/**
 * Turns cursor on.
 *
 * @return None.
 */
void LCDText_CursorOff(void);

/**
 * User define character.
 *
 * @param location    The new character position,
 * @param charmap  The user defined character values.
 *
 * @return None.
 */
void LCDText_CreateChar(unsigned char location, unsigned char charmap[]);

/**
 * Turns display on.
 *
 * @return None.
 */
void LCDText_On(void);
/**
 * Turns display off.
 *
 * @return None.
 */
void LCDText_Off(void);

/**
 * Write a formated string to the LCD
 *
 * @param fmt A printf-style format string, followed by the
 *               variables to use in formating the string.
 */
void LCDText_Printf(const char *fmt,...);

/**
 * @}
 */


/**
 * @}
 */

#endif /* LCDTEXT_H_ */
