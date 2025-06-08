/**
* @file		espserial.h
* @brief	Contains the serial ESP8266 API.
* @version	1.0
* @date		17 Mar 2017
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

#ifndef _ESPSERIAL_H_
#define _ESPSERIAL_H_


/** @addtogroup DRIVERS
 * @{
 */

/** @defgroup ESPSERIAL ESP8266 serial interface
 * @brief This package provides the interface for the ESP8266 with serial interface.
 *
 * @{
 */

/**
 * @brief  Initializes the serial interface for the ESP8266
 * @param  baudrate Set baudrate of device.
 * @return None.
 */
void ESPSERIAL_Init(int baudrate);

/**
 * Write to the ESP8266
 *
 * @param command Buffer data.
 * @param size Lenght of the buffer.
 *
 * @return Number of data that was written.
 */
int ESPSERIAL_Send(void *command, int size);


/**
 * Read from the ESP8266
 *
 * @param response Buffer data.
 * @param maxSize Maximum lenght expected.
 *
 * @return Number of data that was readed.
 */
int ESPSERIAL_Recv(uint8_t *response, int maxSize);

/**
 * @}
 */


/**
 * @}
 */

#endif /* _ESPSERIAL_H_ */
