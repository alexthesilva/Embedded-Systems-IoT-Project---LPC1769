/**
* @file		rtoslcd.h
* @brief	Contains the rtosLCDText API.
* @version	1.0
* @date		17 jun 2023
* @author	Alexandre Silva
*
* Copyright(C) 2023, Alexandre Silva
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
**********************************************************************/

#ifndef _RTOS_LCD_H_
#define _RTOS_LCD_H_

#include <stdint.h>
#include <stdbool.h>
#include "rtoslock.h"
#include "lcdtext.h"

/** @addtogroup FreeRTOS-Drivers
 * @{
 */

/** @defgroup rtosLCD Text LCD
 * @brief This package provides the interface for driving 4-bit HD44780-based LCDs used
 * in an LPCXpresso development board based on LPC1769 from NXP in a multithreaded manner.
 *
 * The LCD data bits are connected to the microcontroller LPC1769 as shown in the following table:
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
#ifndef LCDTEXT_H_

#define LCDText_LINES		LCDText_LINES	/*!< LCD number of lines   */
#define LCDText_COLUMNS		LCDText_LINES	/*!< LCD number of columns */

#endif


#define RTOS_LCD_TASK_STACKSIZE configMINIMAL_STACK_SIZE * 2 /*!< Minimum stack size for the LCD managing task. */
#define RTOS_LCD_QUEUE_SIZE 10 /*!< Queue size for the rtosLCD queue. */


/**
 * Initializes the rtosLCD API.
 *
 * @return	True if successful, false otherwise.
 *
 * @note	This function must be called prior to any other rtosLCDText functions.
 */
bool rtosLCDText_Init();

/**
 * Writes a character to the LCD
 * @param lock Provides the ability to lock access to the display from other tasks(see @ref LockState_t)
 * @param c The character to write to the display
 *
 */
void rtosLCDText_WriteChar(LockState_t lock, char c);

/**
 * Writes a C-string to the LCD
 *
 * @param lock Provides the ability to lock access to the display from other tasks(see @ref LockState_t)
 * @param str The C-string to write to the display
 */
void rtosLCDText_WriteString(LockState_t lock, const char *str);

/**
 * Writes a C-string to specified line of the LCD
 *
 * @param lock Provides the ability to lock access to the display from other tasks(see @ref LockState_t)
 * @param firstLine The C-string to write to the first of display. If NULL nothing is write.
 * @param secondLine The C-string to write to the second of display. If NULL nothing is write.
 *
 * @return None.
 */
void rtosLCDText_WriteLine(LockState_t lock, const char *firstLine, const char *secondLine);


/**
 * Clears the screen and locate cursor to home position (0,0)
 *
 * @param lock Provides the ability to lock access to the display from other tasks(see @ref LockState_t)
 *
 * @return None.
 */
void rtosLCDText_Clear(LockState_t lock);

/**
 * Locates the cursor to a screen line and column
 *
 * @param lock Provides the ability to lock access to the display from other tasks(see @ref LockState_t)
 * @param line    The vertical position from the top, indexed from 0
 * @param column  The horizontal position from the left, indexed from 0
 *
 * @return None.
 */
void rtosLCDText_Locate(LockState_t lock, int line, int column);


/**
 * Turns the cursor on.
 *
 * @param lock Provides the ability to lock access to the display from other tasks(see @ref LockState_t)
 * @return None.
 */
void rtosLCDText_CursorOn(LockState_t lock);

/**
 * Turns cursor off.
 *
 * @param lock Provides the ability to lock access to the display from other tasks(see @ref LockState_t)
 * @return None.
 */
void rtosLCDText_CursorOff(LockState_t lock);

/**
 * User defined character.
 *
 * @param lock Provides the ability to lock access to the display from other tasks(see @ref LockState_t)
 * @param location    The new character position,
 * @param charmap  The user defined character values.
 * @param size The size of the used defined character values buffer.
 *
 * @return None.
 */
void rtosLCDText_CreateChar(LockState_t lock, unsigned char location, unsigned char charmap[], int size);

/**
 * Turns the display on.
 *
 * @param lock Provides the ability to lock access to the display from other tasks(see @ref LockState_t)
 * @return None.
 */
void rtosLCDText_On(LockState_t lock);

/**
 * Turns the display off.
 *
 * @param lock Provides the ability to lock access to the display from other tasks(see @ref LockState_t)
 *
 * @return None.
 */
void rtosLCDText_Off(LockState_t lock);

/**
 * Writes a formated string to the LCD
 *
 * @param lock Provides the ability to lock access to the display from other tasks(see @ref LockState_t)
 * @param fmt A printf-style format string, followed by the
 *               variables to use in formating the string.
 */
void rtosLCDText_Printf(LockState_t lock, const char *fmt, ... );


/**
 * Locks the display without any other action. Particularly useful in situations where
 * the last rtosESPSERIAL function call can vary depending on external factors (i.e. state-machines)
 *
 * @return None.
 */
void rtosLCDText_LockBegin();

/**
 * Unlocks the display without any other action. Particularly useful in situations where
 * the last rtosESPSERIAL function call can vary depending on external factors (i.e. state-machines)
 *
 * @return None.
 */
void rtosLCDText_LockEnd();



/**
 * @}
 */


/**
 * @}
 */

#endif /* _RTOS_LCD_H_ */
