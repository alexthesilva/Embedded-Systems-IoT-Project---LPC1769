/**
* @file		rtosrtc.h
* @brief	Contains the rtosRTC API.
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

#ifndef _RTOS_RTC_H_
#define _RTOS_RTC_H_

/** @addtogroup FreeRTOS-Drivers
 * @{
 */

/** @defgroup rtosRTC Real Time Clock
 * @brief This package provides the interface for the real time clock present in the
 * LPC1769 microcontroller from NXP in a multi-threaded manner.
 *
 * @{
 */

#include <time.h>
#include <stdbool.h>

#define RTOS_RTC_QUEUES_SIZE 5  /*!< Queue size for the rtosRTC queues. */

#define RTOS_RTC_TASK_STACK_SIZE configMINIMAL_STACK_SIZE /*!< Minimum stack size for the RTC managing task. */

/**
 * @brief  Initializes rtosRTC API and starts counting
 * @param  dateTime A pointer to C standard structure tm.
 * @note   If you power off the LPCXpresso board the RTC will stop.
 * @return None.
 */
bool rtosRTC_Init(struct tm *dateTime);

/**
 * @brief  Initializes rtosRTC and starts counting
 * @param  time A C standard time_t value.
 * @note   If you use @ref rtosRTC_Init, do not use this function.
 * @return None.
 */
bool rtosRTC_InitSeconds(time_t time);

/**
 * @brief  Gets date and time from RTC
 * @param  *dateTime A pointer to C standard structure tm to save data to.
 * @return  None.
 */
void rtosRTC_GetValue(struct tm *dateTime);

/**
 * @brief  Sets date and time to RTC
 * @param  *dateTime A pointer to C standard structure tm with date and time
 * @return None.
 */
void rtosRTC_SetValue(struct tm *dateTime);

/**
 * @brief  Gets date and time from RTC
 * @return A C standard time_t with the number of seconds since 01.01.1970 00:00:00
 */
time_t rtosRTC_GetSeconds(void);

/**
 * @brief  Sets date and time from RTC
 * @param time A C standard time_t with the number of seconds since 01.01.1970 00:00:00
 */
void rtosRTC_SetSeconds(time_t time);


/**
 * @}
 */


/**
 * @}
 */

#endif /* _RTC_RTOS_H_ */
