/**
* @file		uimenu.h
* @brief	Contains the uiMENU API.
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


#ifndef _UI_MENU_H_
#define _UI_MENU_H_

/** @addtogroup FreeRTOS-Network-UI
 * @{
 */

/** @defgroup uiMENU Menu interfacing
 * @brief This package provides the means to easily interface menus through an LCD and a rotary encoder.
 *
 * @{
 */

#include <FreeRTOS.h>
#include <time.h>
#include <rtoslcd.h>

#define UI_MENU_ENCODER_INVERT true  /*!< Inverts the rotation values from the encoder, from left to right and vice-versa. */
#define UI_MENU_REFRESH_PERIOD_MS 33  /*!< Refresh period for the menus, in milliseconds. */

#define UI_MENU_BLINK_PERIOD_MS 250  /*!< Blinking period for the menu items, in milliseconds. */

#define UI_MENU_LINES LCDText_LINES  /*!< Ammount of lines in the display used. */
#define UI_MENU_COLUMNS LCDText_COLUMNS /*!< Ammount of columns in the display used. */

#define UI_MENU_SUCCESS_MESSAGE "success" /*!< Message displayed upon successful execution of a given command. */
#define UI_MENU_FAIL_MESSAGE "failure" /*!< Message displayed upon failed execution of a given command. */
#define UI_MENU_PROCESSING_MESSAGE "processing..."  /*!< Message to display while awaiting execution of a given command. */

typedef void (*ItemHandler_t)(void*);  /*!< Handler function type to be used in a menu. */

typedef bool (*CommandFunction_t)(void*);  /*!< Handler function type to be used as a command. */

typedef struct {
	char* menu_name;
	char** item_names;
	ItemHandler_t* handlers;
	void** args;
	int nr_items;
} Menu_t;

/**
 * @brief	Initializes a menu object with the given parameters.
 * @param   menu Menu name to be displayed.
 * @param   item_names Names of the items to be displayed.
 * @param   handlers Handler functions of the items.
 * @param   args Arguments to be sent to item handler functions.
 * @param   nr_items Number of items of the menu.
 * @return	Menu object if successful, NULL otherwise.
 */
Menu_t* uiMENU_Generate(char* menu, char** item_names, ItemHandler_t* handlers, void** args, int nr_items);

/**
 * @brief	Executes a previously generated menu.
 * @param   menu Menu object to be executed.
 * @return	None.
 * @note	uiMENU_Generate should be called prior to this function to in order to generate the menu object.
 *
 */
void uiMENU_Execute(Menu_t* menu);

/**
 * @brief	Executes a given function with bool return type and displays the status of its execution.
 * @param   message_title Name of the command to be executed.
 * @param   function Handler function of the command to be executed.
 * @param   args Argument to the passed handler function.
 * @return	True if the command was executed successfully, false otherwise.
 * @note	uiMENU_Generate should be called prior to this function to in order to generate the menu object.
 *
 */
bool uiMENU_ExecuteCMD(char* message_title, CommandFunction_t function, void* args);

/**
 * @brief	Allows the input of data through a rotary encoder.
 * @param   message_title Name of the input prompt.
 * @param   input_data Input data to display initially, allowing it to be changed. NULL should be passed to start with no data.
 * @param   max_chars Maximum ammount of input characters.
 * @param   char_table String containing the allowable characters to be changed.
 * @param   output_data Modified data after the input prompt.
 * @return	None.
 *
 */
void uiMENU_InputData(char * message_title, char* input_data, int max_chars, char* char_table, char* output_data);

/**
 * @brief	Allows the selection of a given ammount of items.
 * @param   menu_name Name of selection menu.
 * @param   options Names of the items to be displayed.
 * @param   nr_options Ammount of options to be given.
 * @param   cancel Allows the non selection of any of the given menus.
 * @return	Index of the option selected, -1 if an enabled cancellation ocurred.
 *
 */
int uiMENU_SelectOptions(char* menu_name, char** options, int nr_options, bool cancel);

/**
 * @brief	Asks a binary answer question, returning the index of the selected answer.
 * @param   question Question to be asked.
 * @return	Index of the option answer selected.
 *
 */
bool uiMENU_Ask(char* question);


/**
 * @brief	Destroys a previously generated menu object.
 * @param   menu Menu object to be destroyed.
 * @return  None.
 */
void uiMENU_Destroy(Menu_t* menu);

/**
 * @brief	Handles current state based on the encoder interaction with the user.
 * @param   curr_state Address of the current state.
 * @param   state_on_left State to be assigned to the current state upon left/counter-clockwise rotation of the encoder.
 * @param   state_on_right State to be assigned to the current state upon right/clockwise rotation of the encoder.
 * @param   state_on_click State to be assigned to the current state upon single button press of the encoder.
 * @param   state_on_dclick State to be assigned to the current state upon double rapid button press of the encoder.
 * @return  None.
 */
void uiENCODER_Handle(int* curr_state, int state_on_left, int state_on_right, int state_on_click, int state_on_dclick);

/**
 * @brief	Fills a string with empty spaces to prevent junk data from being displayed.
 * @param   string_to_fill String to be filled.
 * @param   col_number Number of collumns of the display being used.
 * @return  Generated string, dynamically allocated.
 */
char * uiMENU_FillString(char * string_to_fill, int col_number);

/**
 * @brief	Centers and fills a string with empty spaces to prevent junk data from being displayed.
 * @param   string_to_center String to be filled.
 * @param   col_number Number of collumns of the display being used.
 * @return  Generated string, dynamically allocated.
 */
char * uiMENU_CenterString(char * string_to_center, int col_number);

/**
 * @brief	Centers fills a string with empty spaces and places arrows in the first and last locations of the passed string.
 * @param   string_to_itemize String to be itemized.
 * @param   col_number Number of collumns of the display being used.
 * @return  Generated string, dynamically allocated.
 */
char * uiMENU_ItemizeString(char * string_to_itemize, int col_number);

/**
 * @brief	Alternates between a string and empty spaces in the positions of the display, length and period given.
 * @param   str_to_blink String to be blinked.
 * @param   line Line position in the display of the string to be blinked.
 * @param   col Column position in the display of the string to be blinked.
 * @param   length_to_blink Length of the string to be blinked.
 * @param   start_time Starting time of a blinking phase, in ticks.
 * @param   blink_period Blinking period of the display.
 * @return  None.
 */
void uiMENU_BlinkStr(char* str_to_blink, int line, int col, int length_to_blink, TickType_t *start_time, TickType_t blink_period);

/**
 * @brief	Ensures the last string displayed is the string given rather than an empty one.
 * @param   str_to_blink String to be displayed.
 * @param   line Line position in the display of the string.
 * @param   col Column position in the display of the string.
 * @param   length_to_blink Length of the string.
 * @return  None.
 */
void uiMENU_BlinkStrClear(char* str_to_blink, int line, int col, int length_to_blink);


/**
 * @}
 */


/**
 * @}
 */

#endif
