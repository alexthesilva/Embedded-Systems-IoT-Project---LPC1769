/**
* @file		info.h
* @brief	Contains information about API version.
* @version	1.0
* @date		19 Mar 2023
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

#ifndef _INFO_H_
#define _INFO_H_

/** @defgroup DRIVERS Drivers
 * @brief This package for drivers.
 * @{
 */

/** @defgroup INFO Library information
 * @brief This package provides the version number of the driver library.
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

