/**
* @file		encoder.h
* @brief	Contains the ENCODER API.
* @version	1.0
* @date		13 set 2022
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

#ifndef _ENCODER_H_
#define _ENCODER_H_

/** @addtogroup DRIVERS
 * @{
 */

/** @defgroup ENCODER Rotary and press button.
 * @brief This package provides the interface for driving the rotary and the push button.
 *
 * The rotary data bits are connect to the microcontroller LPC1769 as show in the follow table:
 *  Rotary Button | LPC1769
 * :--------: | :-----------:
 * CLK | P0.3
 * DT | P0.2
 * SW | P2.13
 * + | 3V3
 *
 * The driver has the following behavior:
 *  i) two steps to vary in the same direction.
 *  i) one step when the inversion is performed.
 *
 *  After initialization its assume that the direction is the clockwise.
 *
 * @{
 */

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


/**
 * @brief Initializes Encoder
 *
 * @return None.
 */
void ENCODER_Init(void);

/**
 * @brief  Gets the value of the push button
 * @return A valid state of the push button (see @ref ENCODER_ButtonValueType
 */
ENCODER_ButtonValueType ENCODER_GetButton(void);


/**
 * @brief  Gets the value of the rotary button
 * @return 0 if not rotate. If rotate returns 1 if was clockwise or -1 if was anticlockwise
 */
int ENCODER_GetValue(void);

/**
 * @}
 */


/**
 * @}
 */

#endif /* ENCODER_H_ */
