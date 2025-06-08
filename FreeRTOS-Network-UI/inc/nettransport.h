/**
* @file		nettransport.h
* @brief	Contains the netTRANSPORT API.
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

#ifndef NETTRANSPORT_H_
#define NETTRANSPORT_H_

/** @addtogroup FreeRTOS-Network-UI
 * @{
 */

/** @defgroup netTRANSPORT Transport layer related functions
 * @brief This package provides the ability to interact with remote servers through transport layer protocols such as TCP and UDP.
 *
 * @{
 */


#include <stdbool.h>

#define NETWORK_UDP_CONNECT_TIMEOUT 10000 /*!< Timeout to be used in the netUDP_Start function, in milliseconds. */
#define NETWORK_TCP_CONNECT_TIMEOUT 10000 /*!< Timeout to be used in the netTCP_Start function, in milliseconds. */
#define NETWORK_RECEIVE_TIMEOUT 20000 /*!< Timeout to be used in the netTRANSPORT_Recv function, in milliseconds. */
#define NETWORK_SEND_TIMEOUT 20000 /*!< Timeout to be used in the netTRANSPORT_Send function, in milliseconds. */

/**
 * @brief	Estabilishes a UDP connection with a remote server.
 * @param   host Host address for the connection.
 * @param   port Host port for the connection.
 *
 * @return	True if successful, false otherwise.
 */
bool netUDP_Start(const char * host, int port);

/**
 * @brief	Closes an ongoing UDP connection.
 * @return	True if successful, false otherwise.
 */
bool netUDP_Close();


/**
 * @brief	Estabilishes a TCP connection with a remote server.
 * @param   host Host address for the connection.
 * @param   port Host port for the connection.
 * @param   keepalive Keepalive time, in seconds, to be used in the connection.
 * @return	True if successful, false otherwise.
 */
bool netTCP_Start(const char *host, const unsigned short int port,
		const unsigned short int keepalive);

/**
 * @brief	Closes an ongoing TCP connection.
 * @return	True if successful, false otherwise.
 */
bool netTCP_Close();

/**
 * @brief	Sends an ammount of data to the desired address through an ongoing transport layer connection.
 * @param   address Host address to send data to.
 * @param   bytes Ammount of bytes to be sent.
 * @return	Number of bytes sent if successful, -1 otherwise.
 */
int netTRANSPORT_Send(unsigned char *address, unsigned int bytes);

/**
 * @brief	Receives an ammount of data to the desired address through an ongoing transport layer connection.
 * @param   address Host address to receive data from.
 * @param   maxbytes Maximum ammount of bytes to be received.
 * @return	Number of bytes received if successful, -1 otherwise.
 */
int netTRANSPORT_Recv(unsigned char *address, unsigned int maxbytes);

/**
 * @}
 */


/**
 * @}
 */

#endif /* NETTRANSPORT_H_ */
