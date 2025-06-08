/**
* @file		netntp.h
* @brief	Contains the netNTP API.
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

#ifndef NETNTP_H_
#define NETNTP_H_


/** @addtogroup FreeRTOS-Network-UI
 * @{
 */

/** @defgroup netNTP NTP
 * @brief This package provides the ability to interact with remote servers through the NTP protocol.
 *
 * @{
 */

#include <time.h>
#include <stdbool.h>

#define NETWORK_NTP_QUEUES_SIZE 10 /*!< Size for the NTP queues. */
#define NETWORK_NTP_TASK_STACK_SIZE configMINIMAL_STACK_SIZE * 6 /*!< Stack size for the NTP task. */

/**
 * @brief	Initializes the netNTP API.
 * @return	True if successful, false otherwise.
 * @note	This function must be called prior to any other netNTP functions.
 */
bool netNTP_Init();

/**
 * @brief	Retrieves the number of seconds elapsed since the year 1900 from an NTP remote server.
 * @param   hour_adjust Number of hours to add or subtract in order to get the desired time.
 * @return	Number of seconds elapsed since 1900 if successful, 0 otherwise.
 */
time_t netNTP_GetTime(int hour_adjust);

/**
 * @}
 */


/**
 * @}
 */

#endif /* NETNTP_H_ */
