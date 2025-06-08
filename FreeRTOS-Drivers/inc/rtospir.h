/**
* @file		rtospir.h
* @brief	Contains the rtosPIR API.
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

#ifndef _RTOS_PIR_H_
#define _RTOS_PIR_H_

/** @addtogroup FreeRTOS-Drivers
 * @{
 */

/** @defgroup PIR Presence detecting sensor.
 * @brief This package provides the interface for driving the presence detecting sensor in a multi-threaded manner.
 *
 * The sensor is connected to the microcontroller LPC1769 in the P2.12 pin.
 *
 * @{
 */

#include <stdint.h>
#include <stdbool.h>

#define RTOS_PIR_PERIOD_MS 100 /*!< Period in milliseconds to update the presence detection value. */

/**
 * @brief Initializes the PIR Presence detecting sensor.
 *
 * @return True if successful, false otherwise.
 */
bool rtosPIR_Init();

/**
 * @brief  Gets the most recent presence detection value.
 *
 * @return True if presence was detected, false otherwise.
 */
bool rtosPIR_GetValue();

/**
 * @}
 */


/**
 * @}
 */

#endif /* _RTOS_PIR_H_ */
