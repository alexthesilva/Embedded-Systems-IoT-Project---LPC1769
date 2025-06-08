#include "encoder.h"
#include "rtosencoder.h"
#include <FreeRTOS.h>
#include <stdbool.h>
#include <semphr.h>
#include <timers.h>

typedef union {
	uint8_t button;
	int8_t encoder;
} ENCODERRecv_t;

static bool init = false;

enum {GETBUTTON, GETVALUE};

static QueueHandle_t encoder_send_queue = NULL;
static QueueHandle_t encoder_recv_queue = NULL;


void rtosENCODER_Task(void *pvParameters){
	ENCODER_Init();
	uint8_t receivedMessage;
	ENCODERRecv_t messageToSend;
	while(1){
        if (xQueueReceive(encoder_send_queue, &receivedMessage, portMAX_DELAY) == pdPASS) {
        	switch (receivedMessage){
				case GETBUTTON:
					messageToSend.button = ENCODER_GetButton();
					xQueueSend(encoder_recv_queue, &messageToSend, portMAX_DELAY);
					break;
				case GETVALUE:
					messageToSend.encoder = ENCODER_GetValue();
					xQueueSend(encoder_recv_queue, &messageToSend, portMAX_DELAY);
					break;
        	}
        }
    }
}


bool rtosENCODER_Init(void){
	if(init)
		return true;

	if((encoder_send_queue = xQueueCreate(RTOS_ENCODER_QUEUES_SIZE, sizeof(uint8_t))) == NULL)
		return false;
	if((encoder_recv_queue = xQueueCreate(RTOS_ENCODER_QUEUES_SIZE, sizeof(ENCODERRecv_t))) == NULL)
		return false;

	if(xTaskCreate(rtosENCODER_Task, "ENCODER_Task", RTOS_ENCODER_TASK_STACK_SIZE, NULL, 1, NULL) != pdPASS)
		return false;

	init = true;
	return true;
}



ENCODER_ButtonValueType rtosENCODER_GetButton(void){
	uint8_t* send  = pvPortMalloc(sizeof(uint8_t));
	*send = GETBUTTON;
	xQueueSend(encoder_send_queue, send, portMAX_DELAY);
	vPortFree(send);

	ENCODER_ButtonValueType ret;
	ENCODERRecv_t* recv  = pvPortMalloc(sizeof(ENCODERRecv_t));
	xQueueReceive(encoder_recv_queue, recv, portMAX_DELAY);
	ret = (ENCODER_ButtonValueType) recv->button;
	vPortFree(recv);
	return ret;

}



ENCODER_RotationValueType rtosENCODER_GetValue(void){
	uint8_t* send  = pvPortMalloc(sizeof(uint8_t));
	*send = GETVALUE;
	xQueueSend(encoder_send_queue, send, portMAX_DELAY);
	vPortFree(send);

	int ret;
	ENCODERRecv_t* recv  = pvPortMalloc(sizeof(ENCODERRecv_t));
	xQueueReceive(encoder_recv_queue, recv, portMAX_DELAY);
	ret = (int) recv->encoder;
	vPortFree(recv);

	return ret;
}
