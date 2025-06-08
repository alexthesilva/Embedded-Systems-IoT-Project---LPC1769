#include "rtosled.h"
#include "led.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

static bool init_state;
static bool init = false;

static QueueHandle_t led_send_queue = NULL;
static QueueHandle_t led_recv_queue = NULL;


enum {GETSTATE, ON, OFF, TOGGLE};

void rtosLED_Task(void *pvParameters){
	LED_Init(init_state);
    uint8_t receivedMessage;
    bool recv;
    while(1) {
        if (xQueueReceive(led_send_queue, &receivedMessage, portMAX_DELAY) == pdPASS) {
        	switch (receivedMessage){
				case GETSTATE:
					recv = LED_GetState();
					xQueueSend(led_recv_queue, &recv, portMAX_DELAY);
					break;
				case ON:
					LED_On();
					break;
				case OFF:
					LED_Off();
					break;
				case TOGGLE:
					LED_Toggle();
					break;
        	}
        }
    }
}

bool rtosLED_Init(bool state){
	if(init)
		return true;
	
	init_state = state;

	if((led_send_queue = xQueueCreate(RTOS_LED_QUEUES_SIZE, sizeof(uint8_t))) == NULL)
		return false;
	if((led_recv_queue = xQueueCreate(RTOS_LED_QUEUES_SIZE, sizeof(bool))) == NULL)
		return false;

	if(xTaskCreate(rtosLED_Task, "LED_Task", RTOS_LED_TASK_STACK_SIZE, NULL, 1, NULL) != pdPASS)
		return false;

	init = true;

	return true;

}


bool rtosLED_GetState(void){
	uint8_t * send = pvPortMalloc(sizeof(uint8_t));
	*send = GETSTATE;
	xQueueSend(led_send_queue, send, portMAX_DELAY);
	vPortFree(send);
	bool recv;
	xQueueReceive(led_recv_queue, &recv, portMAX_DELAY);
	return recv;
}

void rtosLED_On(void){
	uint8_t * send = pvPortMalloc(sizeof(uint8_t));
	*send = ON;
	xQueueSend(led_send_queue, send, portMAX_DELAY);
	vPortFree(send);
}

void rtosLED_Off(void){
	uint8_t * send = pvPortMalloc(sizeof(uint8_t));
	*send = OFF;
	xQueueSend(led_send_queue, send, portMAX_DELAY);
	vPortFree(send);
}


void rtosLED_Toggle(void){
	uint8_t * send = pvPortMalloc(sizeof(uint8_t));
	*send = TOGGLE;
	xQueueSend(led_send_queue, send, portMAX_DELAY);
	vPortFree(send);
}


