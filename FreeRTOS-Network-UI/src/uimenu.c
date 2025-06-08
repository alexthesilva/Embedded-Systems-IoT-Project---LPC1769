#include "uimenu.h"
#include <FreeRTOS.h>
#include <netmqtt.h>
#include <semphr.h>
#include <stdbool.h>
#include "rtosled.h"
#include "rtoslcd.h"
#include "rtosrtc.h"
#include "rtosencoder.h"
#include "rtospir.h"
#include "rtosbh1750.h"
#include <stdio.h>
#include "rtosespserial.h"
#include "netntp.h"
#include "netmqtt.h"
#include "netwifi.h"
#include "string.h"


static TickType_t refresh_rate_tick = pdMS_TO_TICKS(UI_MENU_REFRESH_PERIOD_MS);



Menu_t* uiMENU_Generate(char* menu_name, char** item_names, ItemHandler_t* handlers, void** args, int nr_items){
	Menu_t* ret = pvPortMalloc(sizeof(Menu_t));
	if(ret == NULL)
		return ret;

	ret->menu_name = strdup(menu_name);
	ret->item_names = pvPortMalloc(sizeof(char*) * nr_items);

	for(int i = 0; i < nr_items; i++)
		ret->item_names[i] = uiMENU_FillString(item_names[i], UI_MENU_COLUMNS);

	ret->handlers = handlers;
	ret->args = args;
	ret->nr_items = nr_items;

	return ret;
}


void uiMENU_Destroy(Menu_t* menu){
	for(int i = 0; i < menu->nr_items; i++)
		vPortFree(&(menu->item_names[i]));

	vPortFree(menu);
}


enum{ MENU_ENTRY, MENU_EXIT, MENU_CANCEL,
	  ITEM_ENTRY, ITEM, ITEM_EXEC,
	  ITEM_INC, ITEM_DEC
	};

void uiMENU_Execute(Menu_t* menu){

	TickType_t refresh_start_time;

	int item = 0;
	TickType_t blink_start_time = xTaskGetTickCount();
	int state = MENU_ENTRY;
	bool loop = true;
	while(loop){
		switch(state){
			case MENU_ENTRY:
				rtosLCDText_Clear(LOCK_BEGIN);
				rtosLCDText_Locate(LOCK_CONTINUE, 0, 0);
				rtosLCDText_WriteString(LOCK_END, menu->menu_name);
				state = ITEM_ENTRY;

				break;

			case ITEM_ENTRY:
				uiMENU_BlinkStr(menu->item_names[item], 1, 0, strlen(menu->item_names[item]), &blink_start_time, pdMS_TO_TICKS(UI_MENU_BLINK_PERIOD_MS));
				uiENCODER_Handle(&state, ITEM_DEC, ITEM_INC, ITEM_EXEC, MENU_EXIT);

				break;

			case ITEM_EXEC:
				(*(menu->handlers[item]))(menu->args[item]);
				state = MENU_EXIT;
				break;

			case ITEM_INC:
				item = (item == menu->nr_items - 1) ? 0 : item + 1;
				state = ITEM_ENTRY;
				break;

			case ITEM_DEC:
				item = (item == 0) ? menu->nr_items - 1 : item - 1;
				state = ITEM_ENTRY;
				break;

			case MENU_EXIT:
				rtosLCDText_Clear(LOCK_SINGLE);
				loop = false;
				break;
		}
		refresh_start_time = xTaskGetTickCount();
		vTaskDelayUntil(&refresh_start_time, refresh_rate_tick);
	}
}



int uiMENU_SelectOptions(char* menu_name, char** options, int nr_options, bool cancel){

	char* new_options[nr_options];
	for(int i = 0; i < nr_options; i++)
		new_options[i]  = uiMENU_FillString(options[i], UI_MENU_COLUMNS);
	TickType_t refresh_start_time;

	int item = 0;
	TickType_t blink_start_time = xTaskGetTickCount();
	int state = MENU_ENTRY;
	bool loop = true;
	while(loop){
		switch(state){
			case MENU_ENTRY:
				rtosLCDText_Clear(LOCK_BEGIN);
				rtosLCDText_Locate(LOCK_CONTINUE, 0, 0);
				rtosLCDText_WriteString(LOCK_END, menu_name);
				state = ITEM_ENTRY;

				break;

			case ITEM_ENTRY:
				uiMENU_BlinkStr(new_options[item], 1, 0, strlen(new_options[item]), &blink_start_time, pdMS_TO_TICKS(UI_MENU_BLINK_PERIOD_MS));
				uiENCODER_Handle(&state, ITEM_DEC, ITEM_INC, MENU_EXIT, (cancel) ? MENU_CANCEL : MENU_EXIT);
				break;

			case MENU_CANCEL:
				item = -1;
				state = MENU_EXIT;
				break;

			case ITEM_INC:
				item = (item == nr_options - 1) ? 0 : item + 1;
				state = ITEM_ENTRY;
				break;

			case ITEM_DEC:
				item = (item == 0) ? nr_options - 1 : item - 1;
				state = ITEM_ENTRY;
				break;

			case MENU_EXIT:
				rtosLCDText_Clear(LOCK_SINGLE);
				loop = false;
				break;
		}
		refresh_start_time = xTaskGetTickCount();
		vTaskDelayUntil(&refresh_start_time, refresh_rate_tick);
	}
	for(int i = 0; i < nr_options; i++)
		vPortFree(&(new_options[i]));
	return item;
}


bool uiMENU_Ask(char* question){
	char* options[] = {"no", "yes"};
	return uiMENU_SelectOptions(question, options, 2, false);
}



bool uiMENU_ExecuteCMD(char* message_title, CommandFunction_t function, void* args){

	char* new_title_message = uiMENU_FillString(message_title, UI_MENU_COLUMNS);
	char* processing_message = uiMENU_FillString(UI_MENU_PROCESSING_MESSAGE, UI_MENU_COLUMNS);
	char* success_message = uiMENU_FillString(UI_MENU_SUCCESS_MESSAGE, UI_MENU_COLUMNS);
	char* failure_message = uiMENU_FillString(UI_MENU_FAIL_MESSAGE, UI_MENU_COLUMNS);


	TickType_t refresh_start_time;

	int state = MENU_ENTRY;
	bool loop = true;
	bool ret;
	while(loop){
		switch(state){
			case MENU_ENTRY:
				rtosLCDText_Clear(LOCK_BEGIN);
				rtosLCDText_Locate(LOCK_CONTINUE, 0, 0);
				rtosLCDText_WriteString(LOCK_END, message_title);
				state = ITEM_ENTRY;
				break;

			case ITEM_ENTRY:
				rtosLCDText_Locate(LOCK_BEGIN, 1, 0);
				rtosLCDText_WriteString(LOCK_END, processing_message);
				ret = ((*(function))(args));
				rtosLCDText_Clear(LOCK_BEGIN);
				rtosLCDText_Locate(LOCK_CONTINUE, 0, 0);
				rtosLCDText_WriteString(LOCK_CONTINUE, message_title);
				rtosLCDText_Locate(LOCK_BEGIN, 1, 0);
				rtosLCDText_WriteString(LOCK_END, (ret ? success_message : failure_message));
				state = ITEM_EXEC;
				break;

			case ITEM_EXEC:
				uiENCODER_Handle(&state, state, state, MENU_EXIT, MENU_EXIT);
				break;

			case MENU_EXIT:
				rtosLCDText_Clear(LOCK_SINGLE);
				loop = false;
				break;
		}
		refresh_start_time = xTaskGetTickCount();
		vTaskDelayUntil(&refresh_start_time, refresh_rate_tick);
	}
	vPortFree(new_title_message);
	vPortFree(processing_message);
	vPortFree(success_message);
	vPortFree(failure_message);

	return ret;
}


enum{ DATA_ENTRY, DATA_EXIT,
	  PREPARE_DISPLAY, UPDATE_DISPLAY, SELECT_CHAR,
	  CHAR_INC, CHAR_DEC, CHAR_UPDATE,
	  CHAR_LEFT, CHAR_RIGHT
	};

void uiMENU_InputData(char * message_title, char* input_data, int max_chars, char* char_table, char* output_data){

	char* new_title_message = uiMENU_FillString(message_title, UI_MENU_COLUMNS);
	char* data_change_str = pvPortMalloc(max_chars + 1);
	char* left_data_str = pvPortMalloc(max_chars + 1);
	char* right_data_str = pvPortMalloc(max_chars + 1);
	char* data_str = pvPortMalloc(max_chars + 1);

	if(input_data == NULL)
		memset(data_str, 0, max_chars + 1);
	else
		strcpy(data_str, input_data);
	TickType_t refresh_start_time;
	unsigned int curr_char = 0;
	unsigned int curr_map = 0;
	TickType_t blink_start_time = xTaskGetTickCount();
	int state = DATA_ENTRY;
	bool loop = true;
	char blank_str[] = {0xff, 0};

	unsigned int lcd_offset = 0;
	while(loop){
		switch(state){
			case DATA_ENTRY:
				rtosLCDText_Clear(LOCK_BEGIN);
				rtosLCDText_Locate(LOCK_CONTINUE, 0, 0);
				rtosLCDText_WriteString(LOCK_END, new_title_message);
				state = PREPARE_DISPLAY;

				break;

			case PREPARE_DISPLAY:
				memset(data_change_str, 0, max_chars + 1);
				memset(left_data_str, 0, max_chars + 1);
				memset(right_data_str, 0, max_chars + 1);

				for(int i = 0; i < max_chars; i++)
					data_change_str[i] = (data_str[i] == 0) ? ' ' : data_str[i];

				strncpy(left_data_str, data_change_str, curr_char);
				strncpy(right_data_str, data_change_str + curr_char + 1, max_chars - curr_char - 1);
				state = UPDATE_DISPLAY;
				break;

			case UPDATE_DISPLAY:
				if(curr_char != 0){
					rtosLCDText_Locate(LOCK_BEGIN, 1, 0);
					rtosLCDText_WriteString(LOCK_END, lcd_offset + left_data_str);
				}

				if(data_change_str[curr_char] == ' '){
					uiMENU_BlinkStr(blank_str, 1, curr_char - lcd_offset, 1, &blink_start_time, pdMS_TO_TICKS(UI_MENU_BLINK_PERIOD_MS));
				}
				else
					uiMENU_BlinkStr(data_change_str + curr_char, 1, curr_char - lcd_offset, 1, &blink_start_time, pdMS_TO_TICKS(UI_MENU_BLINK_PERIOD_MS));

				if(curr_char != max_chars - 1){
					rtosLCDText_Locate(LOCK_BEGIN, 1, curr_char + 1 - lcd_offset);
					rtosLCDText_WriteString(LOCK_END, lcd_offset + right_data_str);
				}

				uiENCODER_Handle(&state, CHAR_LEFT, CHAR_RIGHT, (strchr(char_table, data_change_str[curr_char]) != NULL) ? SELECT_CHAR : state, DATA_EXIT);
				break;

			case SELECT_CHAR:
				if(data_change_str[curr_char] == 0){
					uiMENU_BlinkStr(blank_str, 1, curr_char - lcd_offset, 1, &blink_start_time, pdMS_TO_TICKS(UI_MENU_BLINK_PERIOD_MS));
				}

				else
					uiMENU_BlinkStr(data_change_str + curr_char, 1, curr_char - lcd_offset, 1, &blink_start_time, pdMS_TO_TICKS(UI_MENU_BLINK_PERIOD_MS));

				uiENCODER_Handle(&state, CHAR_DEC, CHAR_INC, CHAR_UPDATE, PREPARE_DISPLAY);
				break;


			case CHAR_LEFT:
				if(data_change_str[curr_char] == ' '){
					uiMENU_BlinkStrClear(blank_str, 1, curr_char - lcd_offset, 1);
				}
				else
					uiMENU_BlinkStrClear(data_change_str + curr_char, 1, curr_char - lcd_offset, 1);

				if(curr_char != 0){
					if(lcd_offset > 0)
						--lcd_offset;
					--curr_char;
				}

				state = PREPARE_DISPLAY;
				break;

			case CHAR_RIGHT:
				if(data_change_str[curr_char] == ' '){
					uiMENU_BlinkStrClear(blank_str, 1, curr_char - lcd_offset, 1);
				}
				else
					uiMENU_BlinkStrClear(data_change_str + curr_char, 1, curr_char - lcd_offset, 1);

				if(curr_char < max_chars - 1){
					if(curr_char >= UI_MENU_COLUMNS - 1)
						++lcd_offset;
					++curr_char;
				}

				state = PREPARE_DISPLAY;
				break;


			case CHAR_DEC:
				curr_map = (curr_map == 0) ? strlen(char_table): curr_map - 1;
				data_change_str[curr_char] = char_table[curr_map];

				state = SELECT_CHAR;
				break;

			case CHAR_INC:
				curr_map = (curr_map == strlen(char_table)) ? 0 : curr_map + 1;
				data_change_str[curr_char] = char_table[curr_map];

				state = SELECT_CHAR;
				break;

			case CHAR_UPDATE:
				data_str[curr_char] = data_change_str[curr_char];
				state = PREPARE_DISPLAY;
				break;


			case DATA_EXIT:
				rtosLCDText_Clear(LOCK_SINGLE);
				loop = false;
				break;
		}
		refresh_start_time = xTaskGetTickCount();
		vTaskDelayUntil(&refresh_start_time, refresh_rate_tick);
	}
	strcpy(output_data, data_str);
	vPortFree(new_title_message);
	vPortFree(data_change_str);
	vPortFree(left_data_str);
	vPortFree(right_data_str);
	vPortFree(data_str);
}

char * uiMENU_FillString(char * string_to_fill, int col_number){
	int string_size = strlen(string_to_fill);
	char * res = pvPortMalloc(col_number + 1);
	strncpy(res, string_to_fill, col_number);
	if(string_size < col_number){
		memset(res + string_size, ' ', col_number - string_size);
		res[col_number + 1] = 0;
	}

	return res;
}

char * uiMENU_CenterString(char * string_to_center, int col_number){
	int string_size = strlen(string_to_center);
	char * res = pvPortMalloc(col_number + 1);
	int diff = col_number - string_size;
	int left_cols = diff / 2;
	int right_cols = diff / 2 + diff % 2;
	memset(res, ' ', left_cols);
	strcpy(res + left_cols, string_to_center);
	memset(res + left_cols + string_size, ' ', right_cols);

	return res;
}

char * uiMENU_ItemizeString(char * string_to_itemize, int col_number){
	string_to_itemize[0] = '<';
	string_to_itemize[strlen(string_to_itemize) - 1] = '>';
	return string_to_itemize;
}

void uiENCODER_Handle(int* curr_state, int state_on_left, int state_on_right, int state_on_click, int state_on_dclick){
	int rotation = rtosENCODER_GetValue();
	int button = rtosENCODER_GetButton();
	if(rotation == ROTATE_LEFT)
		*curr_state = (UI_MENU_ENCODER_INVERT) ? state_on_right : state_on_left;
	else if (rotation == ROTATE_RIGHT)
		*curr_state = (UI_MENU_ENCODER_INVERT) ? state_on_left : state_on_right;
	else if (button == BUTTON_CLICKED)
		*curr_state = state_on_click;
	else if (button == BUTTON_DCLICKED)
		*curr_state = state_on_dclick;
}


void uiMENU_BlinkStr(char* str_to_blink, int line, int col, int length_to_blink, TickType_t *start_time, TickType_t blink_period){

	char blank[length_to_blink + 1];
	memset(blank, ' ', length_to_blink);
	blank[length_to_blink] = 0;
	static bool blinkTog;

	if(xTaskGetTickCount() - *start_time > blink_period){
		blinkTog=!blinkTog;
		*start_time = xTaskGetTickCount();

		if(blinkTog){
			rtosLCDText_Locate (LOCK_BEGIN, line, col);
			rtosLCDText_WriteString(LOCK_END, str_to_blink);
		}else{
			rtosLCDText_Locate(LOCK_BEGIN, line , col);
			rtosLCDText_WriteString (LOCK_END, blank);
		}
	}
}

void uiMENU_BlinkStrClear(char* str_to_blink, int line, int col, int length_to_blink){
	char blank[length_to_blink + 1];
	memset(blank, ' ', length_to_blink);
	blank[length_to_blink] = 0;
	rtosLCDText_Locate(LOCK_BEGIN, line , col);
	rtosLCDText_WriteString (LOCK_END, blank);
}
