/**
* @file		rtosbh1750.h
* @brief	Contains the rtosBH1750 API.
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

#ifndef _RTOS_BH1750_H_
#define _RTOS_BH1750_H_

#include "bh1750.h"
#include <stdint.h>
#include <stdbool.h>



/** @addtogroup FreeRTOS-Drivers
 * @{
 */

/** @defgroup rtosBH1750 BH1750
 * @brief This package provides the capabilities to interact with the ambient light sensor BH1750 in a multithreaded manner.
 *
 * The sensor is connected to the microcontroller LPC1769 as shown in the follow table:
 *  BH1750 | LPC1769
 * :--------: | :-----------:
 * SCL  | P0.28
 * SDA  | P0.27
 * ADDR | GND
 * @{
 */


#define RTOS_BH1750_QUEUES_SIZE 5 /*!< Queue size for the rtosBH1750 queues. */
#define RTOS_BH1750_TASK_STACK_SIZE configMINIMAL_STACK_SIZE /*!< Minimum stack size for the BH1750 managing task. */


#ifndef _BH1750_H_
#define _BH1750_H_

/**
  * @brief  BH1750 modes definition.
  */
typedef enum BH1750ModeEnum {
    UNCONFIGURED = UNCONFIGURED,					/*!< Same as Power Down Mode                                              */
    CONTINUOUS_HIGH_RES_MODE = CONTINUOUS_HIGH_RES_MODE,	/*!< Measurement at 1 lux resolution. Measurement time is approx 120ms.   */
    CONTINUOUS_HIGH_RES_MODE_2 = CONTINUOUS_HIGH_RES_MODE_2,	/*!< Measurement at 0.5 lux resolution. Measurement time is approx 120ms. */
    CONTINUOUS_LOW_RES_MODE = CONTINUOUS_LOW_RES_MODE,		/*!< Measurement at 4 lux resolution. Measurement time is approx 16ms.    */
    ONE_TIME_HIGH_RES_MODE = ONE_TIME_HIGH_RES_MODE,		/*!< Measurement at 1 lux resolution. Measurement time is approx 120ms.   */
    ONE_TIME_HIGH_RES_MODE_2 = ONE_TIME_HIGH_RES_MODE_2,	/*!< Measurement at 0.5 lux resolution. Measurement time is approx 120ms. */
    ONE_TIME_LOW_RES_MODE = ONE_TIME_LOW_RES_MODE		/*!< Measurement at 4 lux resolution. Measurement time is approx 16ms.    */
} BH1750_ModeType;

/**
  * @brief  BH1750 measurement times definition.
  */
typedef enum BH1750MeasurementTimeEnum {
	DEFAULT_MEASUREMENT_TIME = DEFAULT_MEASUREMENT_TIME,	/*!< Default measurement time.    */
	MIN_MEASUREMENT_TIME = MIN_MEASUREMENT_TIME,			/*!< Minimum measurement time.    */
	MAX_MEASUREMENT_TIME = MAX_MEASUREMENT_TIME			/*!< Maximum measurement time.    */
} BH1750_MeasurementTimeType;

#endif

/**
 * @brief	Initializes the rtosBH1750 API.
 * @return	True if successful, false otherwise.
 * @note	This function must be called prior to any other rtosBH1750 functions.
 */
bool rtosBH1750_Init(void);

/**
 * @brief	Configures the operation mode.
 * @param	mode: indicate the mode.
 * @return	true if successful, false otherwise.
 */
bool rtosBH1750_ConfigureMode(BH1750_ModeType mode);

/**
 * @brief	Configures the measurement time.
 * @param	time: indicates the measurement time.
 * @return	true if successful, false otherwise.
 */
bool rtosBH1750_SetMeasurementTime(BH1750_MeasurementTimeType time);

/**
 * @brief	Verifies if it is possible to take measurements.
 * @param	maxWait: indicates the measurement time.
 * @return	True if it is possible take measurements, false otherwise.
 */
bool rtosBH1750_Ready(bool maxWait);

/**
 * @brief	Reads the ambient light.
 * @return	Ambient light in lux.
 */
float rtosBH1750_GetLight();


/**
 * @}
 */


/**
 * @}
 */
#endif /* _RTOS_BH1750_H_ */
