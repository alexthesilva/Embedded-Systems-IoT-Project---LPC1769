#include "rtoslcd.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "lcdtext.h"
#include "rtoslock.h"


enum {CHAR, STRING, LINE, CLEAR, LOCATE, CURSOR_ON, CURSOR_OFF, CREATE_CHAR, DISPLAY_ON, DISPLAY_OFF, PRINTF};

typedef struct {
	uint8_t id;

	union{
		struct{
			char * string1;
			char * string2;
		};

		char c;

		struct {
			uint8_t line;
			uint8_t column;
		};

		struct {
			uint8_t location;
			uint8_t *charmap;
		};
	};
} LCDMessage_t;


static LockHandle_t lcd_lock = NULL;
static QueueHandle_t lcd_send_queue = NULL;
static bool init = false;


void rtosLCDText_ReceiverTask(void *pvParameters){
	LCDText_Init();
    LCDMessage_t receivedMessage;


    while(1) {
        if (xQueueReceive(lcd_send_queue, &receivedMessage, portMAX_DELAY) == pdPASS) {
        	switch (receivedMessage.id){
				case CHAR:
					LCDText_WriteChar(receivedMessage.c);
					break;
				case STRING:
					LCDText_WriteString(receivedMessage.string1);
					vPortFree(receivedMessage.string1);
					break;
				case LINE:
					LCDText_WriteLine(receivedMessage.string1, receivedMessage.string2);
					vPortFree(receivedMessage.string1);
					vPortFree(receivedMessage.string2);
					break;
				case CLEAR:
					LCDText_Clear();
					break;
				case LOCATE:
					LCDText_Locate(receivedMessage.line, receivedMessage.column);
					break;
				case CURSOR_ON:
					LCDText_CursorOn();
					break;
				case CURSOR_OFF:
					LCDText_CursorOff();
					break;
				case CREATE_CHAR:
					LCDText_CreateChar(receivedMessage.location, receivedMessage.charmap);
					vPortFree(receivedMessage.charmap);
					break;
				case DISPLAY_ON:
					LCDText_On();
					break;
				case DISPLAY_OFF:
					LCDText_Off();
					break;
				case PRINTF:
					LCDText_WriteString(receivedMessage.string1);
					vPortFree(receivedMessage.string1);
					break;
				default:
					break;
        	}
        }
    }
}



bool rtosLCDText_Init(){
	if(init)
		return true;

	if((lcd_lock = rtosLOCK_Init()) == NULL)
		return false;
	if((lcd_send_queue = xQueueCreate(RTOS_LCD_QUEUE_SIZE, sizeof(LCDMessage_t))) == NULL)
		return false;

	if(xTaskCreate(rtosLCDText_ReceiverTask, "LCD_Receiver", RTOS_LCD_TASK_STACKSIZE, NULL, 1, NULL) != pdPASS)
		return false;

	init = true;
	return true;
}



void rtosLCDText_WriteString(LockState_t lock, const char *str){

	rtosLOCK_Begin(lock, lcd_lock);

	LCDMessage_t * messageToSend = pvPortMalloc(sizeof(LCDMessage_t));

	messageToSend->id = STRING;

	messageToSend->string1 = pvPortMalloc(strlen(str) + 1);
	strcpy(messageToSend->string1, str);
	messageToSend->string1[LCDText_COLUMNS] = 0;

	xQueueSend(lcd_send_queue, messageToSend, portMAX_DELAY);

	vPortFree(messageToSend);

	rtosLOCK_End(lock, lcd_lock);

}


void rtosLCDText_WriteLine(LockState_t lock, const char *firstLine, const char *secondLine){

	rtosLOCK_Begin(lock, lcd_lock);

	LCDMessage_t * messageToSend = pvPortMalloc(sizeof(LCDMessage_t));

	messageToSend->id = LINE;


	messageToSend->string1 = pvPortMalloc(strlen(firstLine) + 1);
	strcpy(messageToSend->string1, firstLine);

	messageToSend->string2 = pvPortMalloc(strlen(secondLine) + 1);
	strcpy(messageToSend->string2, firstLine);

	xQueueSend(lcd_send_queue, messageToSend, portMAX_DELAY);

	vPortFree(messageToSend);

	rtosLOCK_End(lock, lcd_lock);

}


void rtosLCDText_Clear(LockState_t lock){

	rtosLOCK_Begin(lock, lcd_lock);

	LCDMessage_t * messageToSend = pvPortMalloc(sizeof(LCDMessage_t));

	messageToSend->id = CLEAR;
	xQueueSend(lcd_send_queue, messageToSend, portMAX_DELAY);

	vPortFree(messageToSend);

	rtosLOCK_End(lock, lcd_lock);

}




void rtosLCDText_WriteChar(LockState_t lock, char c){

	rtosLOCK_Begin(lock, lcd_lock);
	//msg
	LCDMessage_t * messageToSend = pvPortMalloc(sizeof(LCDMessage_t));

	messageToSend->id = CHAR;
	messageToSend->c = c;
	xQueueSend(lcd_send_queue, messageToSend, portMAX_DELAY);

	vPortFree(messageToSend);

	rtosLOCK_End(lock, lcd_lock);

}


void rtosLCDText_Locate(LockState_t lock, int line, int column){
	rtosLOCK_Begin(lock, lcd_lock);

	LCDMessage_t * messageToSend = pvPortMalloc(sizeof(LCDMessage_t));

	messageToSend->id = LOCATE;
	messageToSend->line = line;
	messageToSend->column = column;

	xQueueSend(lcd_send_queue, messageToSend, portMAX_DELAY);

	vPortFree(messageToSend);

	rtosLOCK_End(lock, lcd_lock);

}


void rtosLCDText_CursorOn(LockState_t lock){

	rtosLOCK_Begin(lock, lcd_lock);

	LCDMessage_t * messageToSend = pvPortMalloc(sizeof(LCDMessage_t));
	messageToSend->id = CURSOR_ON;

	xQueueSend(lcd_send_queue, messageToSend, portMAX_DELAY);

	vPortFree(messageToSend);

	rtosLOCK_End(lock, lcd_lock);

}


void rtosLCDText_CursorOff(LockState_t lock){

	rtosLOCK_Begin(lock, lcd_lock);

	LCDMessage_t * messageToSend = pvPortMalloc(sizeof(LCDMessage_t));

	messageToSend->id = CURSOR_OFF;

	xQueueSend(lcd_send_queue, messageToSend, portMAX_DELAY);

	vPortFree(messageToSend);

	rtosLOCK_End(lock, lcd_lock);

}


void rtosLCDText_CreateChar(LockState_t lock, unsigned char location, unsigned char charmap[], int size){

	rtosLOCK_Begin(lock, lcd_lock);

	LCDMessage_t * messageToSend = pvPortMalloc(sizeof(LCDMessage_t));
	messageToSend->id = CREATE_CHAR;

	messageToSend->charmap = pvPortMalloc(size);
	memcpy(messageToSend->charmap, charmap, size);
	messageToSend->location = location;

	xQueueSend(lcd_send_queue, messageToSend, portMAX_DELAY);

	vPortFree(messageToSend);

	rtosLOCK_End(lock, lcd_lock);

}


void rtosLCDText_On(LockState_t lock){

	rtosLOCK_Begin(lock, lcd_lock);

	LCDMessage_t * messageToSend = pvPortMalloc(sizeof(LCDMessage_t));

	messageToSend->id = DISPLAY_ON;

	xQueueSend(lcd_send_queue, &messageToSend, portMAX_DELAY);

	vPortFree(messageToSend);

	rtosLOCK_End(lock, lcd_lock);

}


void rtosLCDText_Off(LockState_t lock){

	rtosLOCK_Begin(lock, lcd_lock);

	LCDMessage_t * messageToSend = pvPortMalloc(sizeof(LCDMessage_t));
	messageToSend->id = DISPLAY_OFF;

	xQueueSend(lcd_send_queue, messageToSend, portMAX_DELAY);

	vPortFree(messageToSend);

	rtosLOCK_End(lock, lcd_lock);

}


void rtosLCDText_Printf(LockState_t lock, const char *fmt,...){

	rtosLOCK_Begin(lock, lcd_lock);

	LCDMessage_t * messageToSend = pvPortMalloc(sizeof(LCDMessage_t));
	messageToSend->id = PRINTF;
	va_list* ap = pvPortMalloc(sizeof(va_list));
	va_start(*ap, fmt);

	messageToSend->string1 = pvPortMalloc(LCDText_COLUMNS + 1);
	vsprintf(messageToSend->string1, fmt, *ap);
	messageToSend->string1[LCDText_COLUMNS + 1] = 0;
	va_end(*ap);
	vPortFree(ap);

	xQueueSend(lcd_send_queue, messageToSend, portMAX_DELAY);

	vPortFree(messageToSend);

	rtosLOCK_End(lock, lcd_lock);


}

void rtosLCDText_LockBegin(){
	rtosLOCK_Begin(LOCK_BEGIN, lcd_lock);
}


void rtosLCDText_LockEnd(){
	rtosLOCK_End(LOCK_END, lcd_lock);
}


