#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdint.h>
#include "rtosrtc.h"
#include <stdbool.h>
#include <stdio.h>
#include "bh1750.h"
#include "rtosbh1750.h"

typedef struct {
	uint8_t id;
	union {
		BH1750_ModeType mode;
		BH1750_MeasurementTimeType time;
		bool maxWait;
	};
} BH1750Send_t;

typedef struct {
	union{
		bool res;
		float resf;
	};
} BH1750Recv_t;

static bool init = false;

enum {CONFIG, SET, READY, GET};

static QueueHandle_t bh1750_send_queue = NULL;
static QueueHandle_t bh1750_recv_queue = NULL;


void rtosBH1750_Task(void *pvParameters){
	BH1750_Init();
	BH1750Send_t receivedMessage;
	BH1750Recv_t messageToSend;
    while(1){
        if (xQueueReceive(bh1750_send_queue, &receivedMessage, portMAX_DELAY) == pdPASS) {
        	switch (receivedMessage.id){
				case CONFIG:
					messageToSend.res = BH1750_ConfigureMode(receivedMessage.mode);
					xQueueSend(bh1750_recv_queue, &messageToSend, portMAX_DELAY);
					break;
				case SET:
					messageToSend.res = BH1750_SetMeasurementTime(receivedMessage.time);
					xQueueSend(bh1750_recv_queue, &messageToSend, portMAX_DELAY);
					break;
				case READY:
					messageToSend.res = BH1750_Ready(receivedMessage.maxWait);
					xQueueSend(bh1750_recv_queue, &messageToSend, portMAX_DELAY);
					break;
				case GET:
					messageToSend.resf = BH1750_GetLight();
					xQueueSend(bh1750_recv_queue, &messageToSend, portMAX_DELAY);
					break;
        	}
        }
    }
}


bool rtosBH1750_Init(void){
	if(init)
		return true;

	if((bh1750_send_queue = xQueueCreate(RTOS_BH1750_QUEUES_SIZE, sizeof(BH1750Send_t))) == NULL)
		return false;
	if((bh1750_recv_queue = xQueueCreate(RTOS_BH1750_QUEUES_SIZE, sizeof(BH1750Recv_t))) == NULL)
		return false;

	if(xTaskCreate(rtosBH1750_Task, "BH1750_Task", RTOS_BH1750_TASK_STACK_SIZE, NULL, 1, NULL) != pdPASS)
		return false;

	init = true;
	return true;
}



bool rtosBH1750_ConfigureMode(BH1750_ModeType mode){
	BH1750Send_t* send  = pvPortMalloc(sizeof(BH1750Send_t));
	send->id = CONFIG;
	send->mode = mode;
	xQueueSend(bh1750_send_queue, send, portMAX_DELAY);
	vPortFree(send);

	bool ret;
	BH1750Recv_t* recv  = pvPortMalloc(sizeof(BH1750Recv_t));
	xQueueReceive(bh1750_recv_queue, recv, portMAX_DELAY);
	ret = recv->res;
	vPortFree(recv);
	return ret;
}

bool rtosBH1750_SetMeasurementTime(BH1750_MeasurementTimeType time){
	BH1750Send_t* send  = pvPortMalloc(sizeof(BH1750Send_t));
	send->id = SET;
	send->time = time;
	xQueueSend(bh1750_send_queue, send, portMAX_DELAY);
	vPortFree(send);

	bool ret;
	BH1750Recv_t* recv  = pvPortMalloc(sizeof(BH1750Recv_t));
	xQueueReceive(bh1750_recv_queue, recv, portMAX_DELAY);
	ret = recv->res;
	vPortFree(recv);
	return ret;
}


bool rtosBH1750_Ready(bool maxWait){
	BH1750Send_t* send  = pvPortMalloc(sizeof(BH1750Send_t));
	send->id = READY;
	send->maxWait = maxWait;
	xQueueSend(bh1750_send_queue, send, portMAX_DELAY);
	vPortFree(send);

	bool ret;
	BH1750Recv_t* recv  = pvPortMalloc(sizeof(BH1750Recv_t));
	xQueueReceive(bh1750_recv_queue, recv, portMAX_DELAY);
	ret = recv->res;
	vPortFree(recv);
	return ret;
}

float rtosBH1750_GetLight(){
	BH1750Send_t* send  = pvPortMalloc(sizeof(BH1750Send_t));
	send->id = GET;
	xQueueSend(bh1750_send_queue, send, portMAX_DELAY);
	vPortFree(send);

	float ret;
	BH1750Recv_t* recv  = pvPortMalloc(sizeof(BH1750Recv_t));
	xQueueReceive(bh1750_recv_queue, recv, portMAX_DELAY);
	ret = recv->resf;
	vPortFree(recv);
	return ret;
}
