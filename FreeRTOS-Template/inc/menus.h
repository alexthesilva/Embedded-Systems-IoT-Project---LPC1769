/**
* @file		menus.h
* @brief	Contains the menus to be used in the application.
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

#ifndef MENUS_H_
#define MENUS_H_

/** @addtogroup FreeRTOS-Application
 * @{
 */

/** @defgroup appMENU Menus to be interfaced in the application.
 * @brief This package provides the menus to be interfaced in the main application.
 *
 * @{
 */

#include <time.h>

#define MAX_AP_SCAN 5 /*!< Maximum number of AP's to be scanned. */

#define DEFAULT_UTC_ADJUST 1 /*!< Default number of hours to be added to the retrieved synced time. */

/**
 * @brief	Returns the string correspondent to the given week day.
 * @param   index Number of the weekday, starting from 0.
 * @return  Week day string.
 */
char* appMENU_GetWeekDay(int index);

/**
 * @brief	Handles the selection of the SET TIME option in the maintenance mode.
 * @param   arg Arguments passed to the handler.
 * @return  None.
 */
void appMAINTENANCE_SetTimeHandler(void* arg);

/**
 * @brief	Handles the selection of the SET DATE option in the maintenance mode.
 * @param   arg Arguments passed to the handler.
 * @return  None.
 */
void appMAINTENANCE_SetDateHandler(void* arg);

/**
 * @brief	Handles the selection of the SYNC TIME option in the maintenance mode.
 * @param   arg Arguments passed to the handler.
 * @return  None.
 */
void appMAINTENANCE_SyncTimeHandler(void* arg);

/**
 * @brief	Handles the selection of the SET LIGHT option in the maintenance mode.
 * @param   arg Arguments passed to the handler.
 * @return  None.
 */
void appMAINTENANCE_SetLightHandler(void* arg);

/**
 * @brief	Handles the selection of the SET WIFI option in the maintenance mode.
 * @param   arg Arguments passed to the handler.
 * @return  None.
 */
void appMAINTENANCE_SetWifiHandler(void* arg);

/**
 * @brief	Handles the selection of the SET MQTT option in the maintenance mode.
 * @param   arg Arguments passed to the handler.
 * @return  None.
 */
void appMAINTENANCE_SetMQTTHandler(void* arg);


/**
 * @}
 */


/**
 * @}
 */

#endif /* MENUS_H_ */
