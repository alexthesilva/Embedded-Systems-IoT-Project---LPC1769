/**
* @file		rtosled.h
* @brief	Contains the rtosLED API.
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

#ifndef _RTOS_LED_H_
#define _RTOS_LED_H_

#include <stdbool.h>

/** @addtogroup FreeRTOS-Drivers
 * @{
 */

/** @defgroup rtosLED LED
 * @brief This package provides the capabilities to interact with the built-in LED
 *  in the LPCXpresso development board based on LPC1769 from NXP in a multi-threaded manner.
 * @{
 */

#define RTOS_LED_QUEUES_SIZE 5 /*!< Queue size for the rtosLED queues. */

#define RTOS_LED_TASK_STACK_SIZE configMINIMAL_STACK_SIZE /*!< Minimum stack size for the LED managing task. */

/**
 * @brief	Initializes the rtosLED API.
 * @param	state: set LED state: "false" turns LED off and "true" turns LED on.
 * @return	True if successful, false otherwise.
 * @note	This function must be called prior to any other rtosLED functions.
 * The LED will be started with the value passed in the state parameter.
 */
bool rtosLED_Init(bool state);


/**
 * @brief	Get the current LED state.
 * @return	status of LED: "false" indicate LED is off and "true" LED is on.
 */
bool rtosLED_GetState(void);

/**
 * @brief	Turn the LED on
 * @return	None.
 */
void rtosLED_On(void);

/**
 * @brief	Turn the LED off
 * @return	None.
 */
void rtosLED_Off(void);

/**
 * @brief	Toggle the LED
 * @return	None.
 */
void rtosLED_Toggle(void);

/**
 * @}
 */


/**
 * @}
 */

#endif /* _RTOS_LED_H_ */
