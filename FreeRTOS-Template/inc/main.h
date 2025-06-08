/**
* @file		main.h
* @brief	Contains the functions to be used in the main application.
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

#ifndef MAIN_H_
#define MAIN_H_

/** @addtogroup FreeRTOS-Application
 * @{
 */

/** @defgroup appMAIN Main application functions.
 * @brief This package provides the functions to be used in the main application.
 *
 * @{
 */

#include <timers.h>

#define DEFAULT_WIFI_CONNECT false  /*!< Allows the system to connect to a predefined network before booting into normal mode. */

#define DEFAULT_SSID "" /*!< Default SSID of the AP for the system to connect to upon boot, if the respective option was enabled */
#define DEFAULT_PASS "" /*!< Default password of the AP for the system to connect to upon boot, if the respective option was enabled */

#define MQTT_PUBLISH_INTERVAL_S 30 /*!< Default period for the publication of data through MQTT, in seconds. */

#define DEFAULT_CONNECTION_KEEPALIVE_S 	60UL /*!< Default keepalive time for the MQTT connection, in seconds. */
#define DEFAULT_MQTT_ADDRESS			"iot-ps.ddns.net" /*!< Default MQTT server address to connect to. */
#define DEFAULT_MQTT_PORT				1883  /*!< Default MQTT server port to connect to. */
#define DEFAULT_MQTT_DEVICE_TOKEN		"G2-SEIoT"  /*!< Default access token for the MQTT server to connect to. */

/**
 * @brief	Asserts correct initialization of every FreeRTOS-Drivers API.
 * @return  None.
 */
void RTOS_Init();

/**
 * @brief	Asserts correct initialization of every FreeRTOS-Drivers API.
 * @return  None.
 */
void NETWORK_Init();

/**
 * @brief	Asserts correct initialization of every Application layer task.
 * @return  None.
 */
void APPLICATION_Init();

/**
 * @brief	Handler for the display affixing option in normal mode.
 * @param   a Arguments.
 * @return  None.
 */
void appMENU_NormalAffixHandler(void* a);

/**
 * @brief	Light managing ISR.
 * @param   xTimer Handle of the timer being used.
 * @return  None.
 */
void appMENU_NormalManageLightISR(TimerHandle_t xTimer);

/**
 * @brief	MQTT data publishing ISR.
 * @param   xTimer Handle of the timer being used.
 * @return  None.
 */
void appMENU_NormalPublishISR(TimerHandle_t xTimer);

/**
 * @brief	Normal mode task.
 * @param   arg Arguments.
 * @return  None.
 */
void appMENU_NormalMode(void * arg);

/**
 * @brief	Maintenance mode task.
 * @param   arg Arguments.
 * @return  None.
 */
void appMENU_MaintenanceMode(void * arg);

/**
 * @}
 */


/**
 * @}
 */


#endif /* MAIN_H_ */
