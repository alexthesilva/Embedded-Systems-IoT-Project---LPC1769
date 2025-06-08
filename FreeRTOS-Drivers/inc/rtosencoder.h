/**
* @file		rtosencoder.h
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


#ifndef _RTOS_ENCODER_H_
#define _RTOS_ENCODER_H_

#include <stdbool.h>

/** @addtogroup FreeRTOS-Drivers
 * @{
 */

/** @defgroup rtosENCODER ENCODER
 * @brief This package provides the interface for driving the rotary and the push button.
 *
 * The rotary data bits are connected to the LPC1769 microcontroller as shown in the following table:
 *  Rotary Button | LPC1769
 * :--------: | :-----------:
 * CLK | P0.3
 * DT | P0.2
 * SW | P2.13
 * + | 3V3
 *
 * The driver has the following behavior:
 *  i) two steps to vary in the same direction.
 *  i) one step when an inversion is performed.
 *
 *  After initialization clockwise direction is assumed.
 *
 * @{
 */

#define RTOS_ENCODER_QUEUES_SIZE 5 /*!< Queue size for the rtosENCODER queues. */

#define RTOS_ENCODER_TASK_STACK_SIZE configMINIMAL_STACK_SIZE /*!< Minimum stack size for the ENCODER managing task. */

typedef enum RotationEnum {
	ROTATE_NONE = 0,
	ROTATE_LEFT = -1,
	ROTATE_RIGHT = 1,
} ENCODER_RotationValueType;

#ifndef _ENCODER_H_
#define _ENCODER_H_

/**
  * @brief  Push button state structures definition
  */
typedef enum ButtonEnum {
	BUTTON_NOTPRESSED,			/*!< Button not pressed */
	BUTTON_PRESSED,				/*!< Button pressed */
	BUTTON_HELD,				/*!< Button held (still pressed) */
	BUTTON_RELEASE,				/*!< Button released */
	BUTTON_CLICKED,				/*!< Button short pressed and released */
	BUTTON_DCLICKED				/*!< Button double short pressed and released */
} ENCODER_ButtonValueType;

#endif



/**
 * @brief Initializes Encoder
 *
 * @return True if successful, false otherwise
 */
bool rtosENCODER_Init(void);

/**
 * @brief  Gets the value of the push button
 * @return A valid state of the push button (see @ref ENCODER_ButtonValueType
 */
ENCODER_ButtonValueType rtosENCODER_GetButton(void);


/**
 * @brief  Gets the value of the rotary button
 * @return 0 no rotation has beed performed; returns 1 or -1 in case of clockwise and counter-clockwise rotation, respectively.
 */
ENCODER_RotationValueType rtosENCODER_GetValue(void);

/**
 * @}
 */


/**
 * @}
 */

#endif /* _RTOS_ENCODER_H_ */
