/**
* @file		rtosespserial.h
* @brief	Contains the rtosENCODER API.
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

#ifndef _RTOS_ESPSERIAL_H_
#define _RTOS_ESPSERIAL_H_


/** @addtogroup FreeRTOS-Drivers
 * @{
 */

/** @defgroup rtosESPSERIAL ESP8266 serial interface
 * @brief This package provides the interface for the ESP8266 with serial interface in a multi-threaded manner, along with AT command related functions.
 *
 * @{
 */

#include <stdbool.h>
#include <freertos.h>
#include <time.h>
#include "rtoslock.h"

#define RTOS_ESPSERIAL_PRINTF_DEBUG false /*!< Allows sent and received commands to be printed to console when set true */

#define RTOS_ESPSERIAL_DEFAULT_SETUP true /*!< Calls a task to execute the setup function */
#define RTOS_ESPSERIAL_RESTORE_ON_DEFAULT_SETUP false /*!< Restores the ESP8266 restore command in the setup function */
#define RTOS_ESPSERIAL_SETUP_TASK_STACK_SIZE configMINIMAL_STACK_SIZE * 3 /*!< Minimum stack size for the ESP8266 managing task. */

#define RTOS_ESPSERIAL_QUEUES_SIZE 10  /*!< Queue size for the ESP8266 queues. */
#define RTOS_ESPSERIAL_BUFFER_SIZE 512 /*!< Buffer size incoming data transmitted by the ESP8266 device. */

#define RTOS_ESPSERIAL_REFRESH_PERIOD_MS 66 /*!< Execution period for rtosESPSERIAL waiting functions. */
#define RTOS_ESPSERIAL_BUFFERREFRESH_TIMEOUT 500 /*!< Period for the refresh function to wait for incoming data from the ESP8266 device. */
#define RTOS_ESPSERIAL_SHORT_PAUSE 2000 /*!< Maximum timeout in milliseconds for a short pause. */
#define RTOS_ESPSERIAL_LONG_PAUSE  10000 /*!< Maximum timeout in milliseconds for a long pause. */
#define RTOS_ESPSERIAL_SETUP_TIMEOUT 15000  /*!< Maximum timeout in milliseconds for the setup function. */

/**
  * @brief  ESP8266 current state structure definition
  */
typedef enum {START, /*!< Starts waiting for a desired outcome. */
			  QUEUE, /*!< Execution state reached the implemented queue. Only used internally */
			  WAIT, /*!< Waiting for a desired outcome. */
			  FAIL,  /*!< Desired outcome has not been achieved. */
			  SUCCESS /*!< Desired outcome has been achieved. */
} ESPState_t;


/**
 * @brief Initializes the rtosESPSERIAL API.
 * @param baudrate Set baudrate of the device.
 * @return True if successful, false otherwise.
 * @note This function must be called prior to any other rtosESPSERIAL functions.
 *
 */
bool rtosESPSERIAL_Init(int baudrate);


/**
 * @brief Locks the ESP8266 device without any further action. 
 * @return None.
 */
void rtosESPSERIAL_LockBegin();

/**
 * @brief Unlocks the ESP8266 device without any further action. 
 * @return None.
 */
void rtosESPSERIAL_LockEnd();

/**
 * @brief Refreshes the buffer containing read data with any new incoming data.
 * @param lock Provides the ability to lock access to the ESP8266 device from other tasks(see @ref LockState_t)
 * @note This function must be called prior to any other rtosESPSERIAL data-receiving functions.
 */
void rtosESPSERIAL_Refresh(LockState_t lock);

/**
 * @brief Clears the buffer containing read data.
 * @param lock Provides the ability to lock access to the ESP8266 device from other tasks(see @ref LockState_t)
 * @note This function must be called prior to any other rtosESPSERIAL data-receiving functions.
 */
void rtosESPSERIAL_Clear(LockState_t lock);


/** -------------------------
 * @brief Writes data to the ESP8266 device.
 * @param lock Provides the ability to lock access to the ESP8266 device from other tasks(see @ref LockState_t)
 * @param command Buffer data.
 * @param size Length of the buffer.
 * @param bytes_sent Address where the number of total bytes written will be saved to.
 * @return None.
 */
void rtosESPSERIAL_Send(LockState_t lock, void* command, int size, int* bytes_sent);

/** -----------------
 * @brief Reads data from the ESP8266 device.
 * @param lock Provides the ability to lock access to the ESP8266 device from other tasks(see @ref LockState_t)
 * @param dst_buffer Destination buffer for all data to be read into.
 * @param max_bytes Maximum expected length for the destination buffer.
 * @param bytes_read Address where the number of total bytes read will be saved to.
 * @return None.
 */
void rtosESPSERIAL_Recv(LockState_t lock, uint8_t *dst_buffer, int max_bytes, int* bytes_read);



/** ------------------
 * @brief Reads a byte of data from the ESP8266 device.
 * @param lock Provides the ability to lock access to the ESP8266 device from other tasks(see @ref LockState_t)
 * @param dst_buffer Destination for the read byte.
 * @param bytes_read Address where the number of total bytes read will be saved to.
 * @return None.
 */
void rtosESPSERIAL_RecvByte(LockState_t lock, uint8_t *dst_buffer, int* bytes_read);

/**
 * @brief Finds a desired string in the buffer containing read data.
 * @param lock Provides the ability to lock access to the ESP8266 device from other tasks(see @ref LockState_t)
 * @param str Desired string to be found.
 * @param found_str Address of the pointer where the desired string will be saved to; assigned NULL when the desired line is not found.
 * 
 */
void rtosESPSERIAL_FindStr(LockState_t lock, char * str, char ** found_str);


/** 
 * @brief Reads a CR-LF terminated string from the ESP8266 device.
 * @param lock Provides the ability to lock access to the ESP8266 device from other tasks(see @ref LockState_t)
 * @param found_line Address of the pointer where the read line saved to; assigned NULL when the desired line is not found.
 * @return None.
 */
void rtosESPSERIAL_ReadLine(LockState_t lock, char ** found_line);

/**
 * @brief Waits for a desired string to be transmitted by the ESP8266 device.
 * @param lock Provides the ability to lock access to the ESP8266 device from other tasks(see @ref LockState_t)
 * @param expected_str Expected string to be transmitted by the ESP8266 device.
 * @param str_obtained  Address to the pointer to which the read string will be assigned.
 * @param timeout_ms Maximum execution time for the function, in milliseconds.
 * @param espstate Address where the current state of the ESP8266 device will be saved to. (see @ref ESPState_t)
 * @return None.
 */
void rtosESPSERIAL_WaitStr(LockState_t lock, char *expected_str, char **str_obtained, int timeout_ms, ESPState_t* espstate);

/**
 * @brief Waits for a desired response to be transmitted by the ESP8266 device.
 * @param lock Provides the ability to lock access to the ESP8266 device from other tasks(see @ref LockState_t)
 * @param expected_resp Expected response to be transmitted by the ESP8266 device.
 * @param resp_obtained  Address to the pointer to which the read line will be assigned.
 * @param timeout_ms Maximum execution time for the function, in milliseconds.
 * @param espstate Address where the current state of the ESP8266 device will be saved to. (see @ref ESPState_t)
 * @return None.
 */
void rtosESPSERIAL_WaitResp(LockState_t lock, char *expected_resp, char** resp_obtained, int timeout_ms, ESPState_t* espstate);


/**
 * @brief Sends an AT command to the ESP8266 device.
 * @param lock Provides the ability to lock access to the ESP8266 device from other tasks(see @ref LockState_t)
 * @param command Command to be sent; CR-LF is already appended to the given command by the function.
 * @param expected_resp Expected response to be sent by the ESP8266 device.
 * @param timeout_ms Maximum execution time for the function, in milliseconds.
 * @param espstate Address where the current state of the ESP8266 device will be saved to. (see @ref ESPState_t)
 * @return None.
 */
void rtosESPSERIAL_SendAT(LockState_t lock, char * command, char * expected_resp, int timeout_ms, ESPState_t* espstate);



/**
 * @brief Sets up the ESP8266 device with RST/RESTORE, ATE=0, AT+CWMODE=1, AT+CWLAPOPT=1,127 and CIPMUX=0 commands.
 * @param restore RESTORE command will be executed
 * @return true if successful, false otherwise.
 */
bool rtosESPSERIAL_Setup(bool restore);


/**
 * @}
 */


/**
 * @}
 */
#endif /* _RTOS_ESPSERIAL_H_ */
