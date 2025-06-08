/**
* @file		rtostimeout.h
* @brief	Contains the rtosTIMEOUT API.
* @version	1.0
* @date		12 jul 2023
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

#ifndef _RTOS_TIMEOUT_H_
#define _RTOS_TIMEOUT_H_

/** @addtogroup FreeRTOS-Drivers
 * @{
 */

/** @defgroup rtosTIMEOUT Timeout
 * @brief This package provides the API to execute non-blocking timeouts in FreeRTOS.
 *
 * @{
 */

#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>

/**
 * @brief  Initializes a timeout.
 * @return Starting tick ammount.
 */
TickType_t rtosTIMEOUT_Start();

/**
 * @brief  Checks if a timeout has occurred.
 * @param  start_time Tick ammount of a previously initiated timeout.
 * @param  timeout Timeout period to check for, in ticks.
 * @return True whenever a timeout has occurred, false otherwise.
 */
bool rtosTIMEOUT_Expired(TickType_t start_time, TickType_t timeout);

/**
 * @}
 */


/**
 * @}
 */

#endif
