/**
* @file		netwifi.h
* @brief	Contains the netWIFI API.
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

#ifndef NETWIFI_H_
#define NETWIFI_H_

/** @addtogroup FreeRTOS-Network-UI
 * @{
 */

/** @defgroup netWIFI WiFi related functions
 * @brief This package provides the ability to interact with remote access points through WiFi protocols.
 *
 * @{
 */



typedef struct{
	char* ssid;
	int rssi;
	int channel;
	int ecn;
	char *mac;
	int freq_offset;
	int freq_calibration;
} Network_t;


#include <stdbool.h>

#define NETWORK_REFRESH_PERIOD_MS 66  /*!< Refresh period for the WiFi task, in milliseconds. */
#define NETWORK_WIFI_QUEUES_SIZE 10 /*!< Size for the WiFi queues. */
#define NETWORK_WIFI_STACK_SIZE configMINIMAL_STACK_SIZE * 9 /*!< Stack size for the WiFi task. */

#define NETWORK_MAX_SSID_LENGTH 32 /*!< Maximum length for a WiFi SSID. */
#define NETWORK_MAX_PASSWORD_LENGTH 64 /*!< Maximum length for a WiFi password. */
#define NETWORK_MAC_ADDRESS_LENGTH 18 /*!< Maximum length for a WiFi mac address. */
#define NETWORK_MAX_BSSID_LENGTH 18 /*!< Maximum length for a WiFi BSSID. */

#define NETWORK_CONNECT_TIMEOUT 20000 /*!< Timeout to be used in the netWIFI_Connect function, in milliseconds. */
#define NETWORK_WIFI_SCAN_TIMEOUT 20000 /*!< Timeout to be used in the netWIFI_Scan function, in milliseconds. */
#define NETWORK_WIFI_CHECK_PERIOD 5000 /*!< Timeout to be used in the netWIFI_Check function, in milliseconds. */

/**
 * @brief	Initializes the netWIFI API.
 * @return	True if successful, false otherwise.
 * @note	This function must be called prior to any other netWIFI functions.
 */
bool netWIFI_Init();

/**
 * @brief	Estabilishes a WiFi connection with an access point.
 * @param   ssid SSID of the access point.
 * @param   pass Password of the access point.
 * @param   makedefault Connects to the indicated AP every time the WiFi module boots.
 * @return	True if successful, false otherwise.
 */
bool netWIFI_Connect(char * ssid, char *pass, bool makedefault);

/**
 * @brief	Disconnects from a previously connected access point.
 * @return	True if successful, false otherwise.
 */
bool netWIFI_Disconnect();

/**
 * @brief	Checks whether a usable WiFi connection is estabilished.
 * @param   result Address where the result of the operation will be written to: true if successful, false otherwise.
 */
void netWIFI_Check(bool* result);

/**
 * @brief	Scans the available access points and stores the list in memory.
 * @param   max_ap Maximum ammount of AP's to store.
 * @param   nets_scanned Address where the number of scanned AP's will be written to.
 * @return	None.
 */
void netWIFI_Scan(int max_ap, int* nets_scanned);

/**
 * @brief	Retrieves a scanned available access point previously stored in memory.
 * @param   index Index of the scanned network stored in memory.
 * @return	Scanned network if successfull, NULL otherwise.
 * @note	This function must be called after netWIFI_Scan, otherwise undefined behaviour is to be expected.
 *
 */
Network_t* netWIFI_ScannedSSID(int index);


/**
 * @}
 */


/**
 * @}
 */


#endif /* NETWIFI_H_ */
