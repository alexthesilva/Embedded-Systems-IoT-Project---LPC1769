/**
* @file		led.h
* @brief	Contains the LED API.
* @version	1.0
* @date		9 Out 2018
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

#ifndef _LED_H_
#define _LED_H_

/** @addtogroup DRIVERS
 * @{
 */


/** @defgroup LED LED
 * @brief This package provides the capabilities such as on/off/toggle to the LED
 *  in LPCXpresso development board based on LPC1769 from NXP.
 * @{
 */

/**
 * @brief	Initializes the LED API.
 * @param	state: set LED state: "false" turns LED off and "true" turns LED on.
 * @return	None.
 * @note	This function must be called prior to any other LED functions.
 * The LED will started in the value passed in the parameter.
 */
void LED_Init(bool state);


/**
 * @brief	Get LED state. 
 * @return	status of LED: "false" indicate LED is off and "true" LED is on.
 */
bool LED_GetState(void);

/**
 * @brief	Turn LED on
 * @return	None.
 */
void LED_On(void);

/**
 * @brief	Turn LED off
 * @return	None.
 */
void LED_Off(void);

/**
 * @brief	Toggle LED
 * @return	None.
 */
void LED_Toggle(void);

/**
 * @}
 */


/**
 * @}
 */

#endif /* _LED_H_ */

