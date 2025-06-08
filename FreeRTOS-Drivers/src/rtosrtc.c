#include "FreeRTOS.h"
#include "semphr.h"
#include "rtosrtc.h"
#include "rtc.h"
#include <stdbool.h>
#include <time.h>


static struct tm * datetime;
static time_t seconds;
static bool init_seconds = false;
static bool init = false;

static QueueHandle_t rtc_send_queue = NULL;
static QueueHandle_t rtc_recv_queue = NULL;

typedef struct {
	uint8_t id;
	union{
		struct tm * datetime;
		time_t seconds;
	};
} RTCSend_t;

enum {GETVALUE, SETVALUE, GETSECONDS, SETSECONDS};

void rtosRTC_Task(void *pvParameters){
	if(init_seconds)
		RTC_InitSeconds(seconds);
	else
		RTC_Init(datetime);

	RTCSend_t receivedMessage;
    time_t recv;
    while(1) {
        if (xQueueReceive(rtc_send_queue, &receivedMessage, portMAX_DELAY) == pdPASS) {
        	switch (receivedMessage.id){
				case GETVALUE:
					RTC_GetValue(receivedMessage.datetime);
					xQueueSend(rtc_recv_queue, &recv, portMAX_DELAY);
					break;
				case SETVALUE:
					RTC_SetValue(receivedMessage.datetime);
					xQueueSend(rtc_recv_queue, &recv, portMAX_DELAY);
					break;
				case GETSECONDS:
					recv = RTC_GetSeconds();
					xQueueSend(rtc_recv_queue, &recv, portMAX_DELAY);
					break;
				case SETSECONDS:
					RTC_SetSeconds(receivedMessage.seconds);
					xQueueSend(rtc_recv_queue, &recv, portMAX_DELAY);
					break;
        	}
        }
    }
}



bool rtosRTC_Init(struct tm *dateTime){
	if(init)
		return true;

	init_seconds = false;
	datetime = dateTime;

	if((rtc_send_queue = xQueueCreate(RTOS_RTC_QUEUES_SIZE, sizeof(RTCSend_t))) == NULL)
		return false;
	if((rtc_recv_queue = xQueueCreate(RTOS_RTC_QUEUES_SIZE, sizeof(time_t))) == NULL)
		return false;

	if(xTaskCreate(rtosRTC_Task, "RTC_Task", RTOS_RTC_TASK_STACK_SIZE, NULL, 1, NULL) != pdPASS)
		return false;

	init = true;
	return true;
}


bool rtosRTC_InitSeconds(time_t time){
	if(init)
		return true;

	init_seconds = true;
	seconds = time;

	if((rtc_send_queue = xQueueCreate(RTOS_RTC_QUEUES_SIZE, sizeof(RTCSend_t))) == NULL)
		return false;
	if((rtc_recv_queue = xQueueCreate(RTOS_RTC_QUEUES_SIZE, sizeof(time_t))) == NULL)
		return false;
	vQueueAddToRegistry(rtc_send_queue, "RTCSENDQ");
	vQueueAddToRegistry(rtc_recv_queue, "RTCRECVQ");

	xTaskCreate(rtosRTC_Task, "RTC_Task", RTOS_RTC_TASK_STACK_SIZE, NULL, 1, NULL);

	init = true;
	return true;
}


void rtosRTC_GetValue(struct tm *dateTime){
	RTCSend_t * send  = pvPortMalloc(sizeof(RTCSend_t));
	send->id = GETVALUE;
	send->datetime = dateTime;
	xQueueSend(rtc_send_queue, send, portMAX_DELAY);
	vPortFree(send);

	struct tm recv;
	xQueueReceive(rtc_recv_queue, &recv, portMAX_DELAY);

}

void rtosRTC_SetValue(struct tm *dateTime){
	RTCSend_t * send  = pvPortMalloc(sizeof(RTCSend_t));
	send->id = SETVALUE;
	send->datetime = dateTime;
	xQueueSend(rtc_send_queue, send, portMAX_DELAY);
	vPortFree(send);

	struct tm recv;
	xQueueReceive(rtc_recv_queue, &recv, portMAX_DELAY);
}


time_t rtosRTC_GetSeconds(void){
	RTCSend_t * send  = pvPortMalloc(sizeof(RTCSend_t));
	send->id = GETSECONDS;
	xQueueSend(rtc_send_queue, send, portMAX_DELAY);
	vPortFree(send);

	time_t dst;
	xQueueReceive(rtc_recv_queue, &dst, portMAX_DELAY);
	return dst;

}


void rtosRTC_SetSeconds(time_t time){
	RTCSend_t * send  = pvPortMalloc(sizeof(RTCSend_t));
	send->id = SETSECONDS;
	send->seconds = time;
	xQueueSend(rtc_send_queue, send, portMAX_DELAY);
	vPortFree(send);
	struct tm recv;
	xQueueReceive(rtc_recv_queue, &recv, portMAX_DELAY);

}
