/**
* @file		netmqtt.h
* @brief	Contains the netMQTT API.
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

#ifndef NETMQTT_H_
#define NETMQTT_H_

/** @addtogroup FreeRTOS-Network-UI
 * @{
 */

/** @defgroup netMQTT MQTT
 * @brief This package provides the ability to interact with remote servers through the MQTT protocol.
 *
 * @{
 */


#include <stdbool.h>
#include <time.h>

#define NETWORK_MQTT_REFRESH_PERIOD_MS 66  /*!< Refresh period for the MQTT task, in milliseconds. */
#define NETWORK_MQTT_TASK_STACK_SIZE configMINIMAL_STACK_SIZE * 9  /*!< Stack size for the MQTT task. */
#define NETWORK_MQTT_QUEUES_SIZE 10  /*!< Size for the MQTT queues. */
#define NETWORK_MQTT_ADDRESS_LENGTH 64  /*!< Maximum length for the MQTT address length. */
#define NETWORK_MQTT_DEVICE_TOKEN_LENGTH 64  /*!< Maximum length for the MQTT address length. */
#define MQTT_NETWORK_CONNECT_TIMEOUT 7000  /*!< Maximum delay to wait at boot, in milliseconds, for WiFi connection to be estabilished. */

/**
 * @brief	Initializes the netMQTT API.
 * @return	True if successful, false otherwise.
 * @note	This function must be called prior to any other netMQTT functions.
 */
bool netMQTT_Init();

/**
 * @brief	Checks if the MQTT service is connected to the server and ready to be used.
 * @return	True if ready, false otherwise.
 */
bool netMQTT_Ready();

/**
 * @brief	Configures the credentials to be used in the MQTT protocol.
 * @param   address Address of the remote server to connect to.
 * @param   token Access token to access the remote server.
 * @param   port Port to be used to connect the remote server.
 * @param   keepalive Keepalive time, in seconds, to be used in the connection.
 * @return	True if ready, false otherwise.
 */
void netMQTT_Config(char* address, char* token, unsigned long port, unsigned long keepalive);

/**
 * @brief	Starts the MQTT task.
 * @return	None.
 */
void netMQTT_Start();

/**
 * @brief	Stops the MQTT task.
 * @return	None.
 */
void netMQTT_Stop();

/**
 * @brief	Publishes data to the previously configured MQTT server.
 * @return	True if successful, false otherwise.
 */
bool netMQTT_Publish(char * key, char * value);


/**
 * @}
 */


/**
 * @}
 */

#endif /* NETMQTT_H_ */
