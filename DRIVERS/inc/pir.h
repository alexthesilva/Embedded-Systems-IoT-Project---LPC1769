/**
* @file		pir.h
* @brief    Contains the presence detect sensor API.
* @version	1.0
* @date		13 set 2022
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

#ifndef _PIR_H_
#define _PIR_H_

/** @addtogroup DRIVERS
 * @{
 */

/** @defgroup PIR Presence detect sensor.
 * @brief This package provides the interface for driving the presence detect sensor.
 *
 * The sensor is connect to the microcontroller LPC1769 in the P2.12 pin.
 *
 * @{
 */

/**
 * @brief Initializes Encoder
 *
 * @return None.
 */
void PIR_Init(void);

/**
 * @brief  Get if was detected presence or not.
 *
 * @return true if presence was detected or false if not
 */
bool PIR_GetValue(void);


/**
 * @}
 */


/**
 * @}
 */

#endif /* _PIR_H_ */
