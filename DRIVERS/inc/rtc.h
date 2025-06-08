/**
* @file		rtc.h
* @brief	Contains the LED API.
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

#ifndef _RTC_H_
#define _RTC_H_

/** @addtogroup DRIVERS
 * @{
 */

/** @defgroup RTC Real Time Clock
 * @brief This package provides the interface for the real time clock present in the
 * microcontroller LPC1769 from NXP.
 *
 * @{
 */

/**
 * @brief  Initializes RTC and starts counting
 * @param  dateTime A pointer to C standard structure tm.
 * @note   If you power off the LPCXpresso board the RTC will stop.
 * @return None.
 */
void RTC_Init(struct tm *dateTime);

/**
 * @brief  Initializes RTC and starts counting
 * @param  time A C standard time_t value.
 * @note   If you use @ref RTC_Init not use this function.
 * @return None.
 */
void RTC_InitSeconds(time_t time);

/**
 * @brief  Gets date and time from RTC
 * @param  *dateTime A pointer to C standard structure tm to save data to.
 * @return  None.
 */
void RTC_GetValue(struct tm *dateTime);

/**
 * @brief  Sets date and time to RTC
 * @param  *dateTime A pointer to C standard structure tm with date and time
 * @return None.
 */
void RTC_SetValue(struct tm *dateTime);

/**
 * @brief  Gets date and time from RTC
 * @return A C standard time_t with the number of seconds since 01.01.1970 00:00:00
 */
time_t RTC_GetSeconds(void);

/**
 * @brief  Sets date and time from RTC
 * @param time A C standard time_t with the number of seconds since 01.01.1970 00:00:00
 */
void RTC_SetSeconds(time_t time);


/**
 * @}
 */


/**
 * @}
 */

#endif /* _RTC_H_ */
