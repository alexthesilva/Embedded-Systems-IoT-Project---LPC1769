/**
* @file		bh1750.h
* @brief	Contains the BH1750 ambient light sensor API.
* @version	1.0
* @date		9 Out 2021
* @author	PSampaio
*
* Copyright(C) 2015-2022, PSampaio
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
**********************************************************************/

#ifndef _BH1750_H_
#define _BH1750_H_

/** @addtogroup DRIVERS
 * @{
 */

/** @defgroup BH1750 BH1750
 * @brief This package provides the capabilities interact with the ambient light sensor BH1750.
 *
 * The sensor is connect to the microcontroller LPC1769 as show in the follow table:
 *  BH1750 | LPC1769
 * :--------: | :-----------:
 * SCL  | P0.28
 * SDA  | P0.27
 * ADDR | GND
 * @{
 */

typedef enum BH1750ModeEnum {
    UNCONFIGURED = 0,					/*!< Same as Power Down Mode                                              */
    CONTINUOUS_HIGH_RES_MODE = 0x10,	/*!< Measurement at 1 lux resolution. Measurement time is approx 120ms.   */
    CONTINUOUS_HIGH_RES_MODE_2 = 0x11,	/*!< Measurement at 0.5 lux resolution. Measurement time is approx 120ms. */
    CONTINUOUS_LOW_RES_MODE = 0x13,		/*!< Measurement at 4 lux resolution. Measurement time is approx 16ms.    */
    ONE_TIME_HIGH_RES_MODE = 0x20,		/*!< Measurement at 1 lux resolution. Measurement time is approx 120ms.   */
    ONE_TIME_HIGH_RES_MODE_2 = 0x21,	/*!< Measurement at 0.5 lux resolution. Measurement time is approx 120ms. */
    ONE_TIME_LOW_RES_MODE = 0x23		/*!< Measurement at 4 lux resolution. Measurement time is approx 16ms.    */
} BH1750_ModeType;

typedef enum BH1750MeasurementTimeEnum {
	DEFAULT_MEASUREMENT_TIME = 69,		/*!< Default measurement time.    */
	MIN_MEASUREMENT_TIME = 31,			/*!< Minimum measurement time.    */
	MAX_MEASUREMENT_TIME = 254			/*!< Maximum measurement time.    */
} BH1750_MeasurementTimeType;

/**
 * @brief	Initializes the BH1750 API.
 * @return	None.
 * @note	This function must be called prior to any other BH1750 functions.
 */
void BH1750_Init(void);

/**
 * @brief	Configure the operation mode.
 * @param	mode: indicate the mode.
 * @return	true if success, otherwise false.
 */
bool BH1750_ConfigureMode(BH1750_ModeType mode);

/**
 * @brief	Configure the measurement time.
 * @param	time: indicate the measurement time.
 * @return	true if success, otherwise false.
 */
bool BH1750_SetMeasurementTime(BH1750_MeasurementTimeType time);

/**
 * @brief	Verify if it is possible do a measures.
 * @param	maxWait: indicate the measurement time.
 * @return	true if it is possible do a measures, otherwise false.
 */
bool BH1750_Ready(bool maxWait);

/**
 * @brief	Read the ambient light.
 * @return	Ambient light in lux.
 */
float BH1750_GetLight();

/**
 * @}
 */


/**
 * @}
 */
#endif /* _BH1750_H_ */
