/**
* @file		rtoslock.h
* @brief	Contains the rtosLOCK API.
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


#ifndef _RTOSLOCK_H_
#define _RTOSLOCK_H_

/** @addtogroup FreeRTOS-Drivers
 * @{
 */

/** @defgroup rtosLOCK Task locking mechanism for FreeRTOS
 * @brief This package provides the ability for a calling task
 * to lock execution of the section of code placed in between
 * rtosLOCK_Begin and rtosLOCK_End function calls from other tasks.
 *
 * @{
 */

#include <stdbool.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

struct LockStruct;

/**
  * @brief  Lock handle structure definition.
  */
typedef struct LockStruct * LockHandle_t;

/**
  * @brief  Lock state definition.
  * LOCK_NONE or LOCK_SINGLE can be executed independently.
  * LOCK_BEGIN and LOCK_END should be called in the beginning and end, respectively, of a code section
  * to be locked, with LOCK_CONTINUE being used in between the last two, if needed.
  */
typedef enum {
	LOCK_NONE, /*!< Ignores the locking mechanism */
	LOCK_BEGIN, /*!< Begins locking of a code section */
	LOCK_CONTINUE, /*!< Continues locking of a code section */
	LOCK_END, /*!< Ends locking of a code section */
	LOCK_SINGLE /*!< Begins and ends locking of the code section in a single call  */
} LockState_t;


/**
 * @brief Initializes the rtosLOCK API.
 *
 * @return Lock handle upon success, NULL otherwise.
 */
LockHandle_t rtosLOCK_Init();

/**
 * @brief Allows the beginning of a code section lock for an initialized lock handle.
 *
 * @param lock Provides the ability to lock access to the code section until rtosLOCK_End function call(see @ref LockState_t)
 * @param handle Lock handle  (see @ref LockHandle_t)
 * @return None.
 * @note Undefined behaviour whenever the lock sequence is not executed in the proper order.
 *
 */
void rtosLOCK_Begin(LockState_t lock, LockHandle_t handle);

/**
 * @brief Allows the end of a code section lock for an initialized lock handle.
 *
 * @param lock Provides the ability to end the locking of the code section previously begun by the rtosLOCK_Begin function call(see @ref LockState_t)
 * @param handle Lock handle  (see @ref LockHandle_t)
 *
 *
 * @return None.
 * @note Undefined behaviour whenever the lock sequence is not executed in the proper order.
 *
 */
void rtosLOCK_End(LockState_t lock, LockHandle_t handle);

/**
 * @brief Destroys an initialized lock handle.
 *
 * @param handle Lock handle  (see @ref LockHandle_t)
 * @return None.
 *
 */
void rtosLOCK_Destroy(LockHandle_t handle);


/**
 * @}
 */


/**
 * @}
 */

#endif /* _RTOSLOCK_H_ */
