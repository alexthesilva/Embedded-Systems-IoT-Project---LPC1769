/**
* @file		info.h
* @brief	Contains information about the current API version.
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

#ifndef _INFO_H_
#define _INFO_H_

/** @defgroup FreeRTOS-Drivers FreeRTOS-Drivers
 * @brief This package is set for use with FreeRTOS-Drivers.
 * @{
 */

/** @defgroup INFO Library information
 * @brief This package provides the version number of FreeRTOS-Drivers library.
 * @{
 */

/**
 * Get version number of the library.
 * @return	Version number.
 */
int INFO_GetVersion(void);

/**
 * @}
 */


/**
 * @}
 */

#endif /* _INFO_H_ */

