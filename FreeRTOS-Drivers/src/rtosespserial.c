#include "rtosespserial.h"
#include <stdbool.h>
#include <FreeRTOS.h>
#include <stdint.h>
#include <queue.h>
#include <semphr.h>
#include <string.h>
#include <stdio.h>
#include "rtoslock.h"
#include "espserial.h"
#include "rtostimeout.h"


typedef struct {
	uint8_t id;

	union{
		struct {
			void* command;
			int size;
		} send;

		struct {
			 uint8_t* dst_buffer;
			 int max_bytes;
		} recv;

		struct {
			char **line;
		} readline;

		struct {
			char *str;
			char **line;
		} findstr;

		struct {
			int timeout_ms;
			char* expected_resp;
			char** str_obtained;
			ESPState_t* espstate;
		} waitstr;

		struct {
			int timeout_ms;
			char* expected_resp;
			char** resp_obtained;
			ESPState_t* espstate;
		} waitresp;

		struct {
			char* command;
			char* expected_resp;
			int timeout_ms;
			ESPState_t* espstate;
		} at;
	};

} ESPSend_t;

typedef struct {

	union{
		int bytes_sent;
		int bytes_read;
	};

} ESPRecv_t;

static void rtosESPSERIAL_SetupTask(void * a);


enum {REFRESH, SEND, RECV, CLEAR, READLINE, FINDSTR, WAITRESP, WAITSTR, AT};


static LockHandle_t espserial_lock = NULL;

static int baud = 0;

static uint8_t ESPSERIALBuffer[RTOS_ESPSERIAL_BUFFER_SIZE];
static int available_bytes = 0;
static int head;
static int tail;

static QueueHandle_t espserial_send_queue = NULL;
static QueueHandle_t espserial_recv_queue = NULL;


static bool init = false;

static void rtosESPSERIAL_Task(void *pvParameters);


static TickType_t refresh_rate_tick = pdMS_TO_TICKS(RTOS_ESPSERIAL_REFRESH_PERIOD_MS);



bool rtosESPSERIAL_Init(int baudrate){
	if(init)
		return true;

	baud = baudrate;

	if((espserial_lock = rtosLOCK_Init()) == NULL)
		return false;

	if((espserial_send_queue = xQueueCreate(RTOS_ESPSERIAL_QUEUES_SIZE, sizeof(ESPSend_t))) == NULL)
		return false;

	memset(ESPSERIALBuffer, 0, RTOS_ESPSERIAL_BUFFER_SIZE);

	if((espserial_recv_queue = xQueueCreate(RTOS_ESPSERIAL_QUEUES_SIZE, sizeof(ESPRecv_t))) == NULL)
		return false;

	if((xTaskCreate(rtosESPSERIAL_Task, "ESPSERIAL_Task", RTOS_ESPSERIAL_SETUP_TASK_STACK_SIZE, NULL, 1, NULL)) != pdPASS)
		return false;

	vQueueAddToRegistry(espserial_send_queue, "SENDQ");
	vQueueAddToRegistry(espserial_recv_queue, "RECVQ");

	if(RTOS_ESPSERIAL_DEFAULT_SETUP){
		if((xTaskCreate(rtosESPSERIAL_SetupTask, "ESPSERIAL_SetupTask", configMINIMAL_STACK_SIZE * 3, NULL, 1, NULL)) != pdPASS)
			return false;
	}


	init = true;
	return true;
}

void rtosESPSERIAL_LockBegin(){
	rtosLOCK_Begin(LOCK_BEGIN, espserial_lock);
}

void rtosESPSERIAL_LockEnd(){
	rtosLOCK_End(LOCK_END, espserial_lock);
}


void rtosESPSERIAL_Refresh(LockState_t lock){

	rtosLOCK_Begin(lock, espserial_lock);

	ESPSend_t * a = pvPortMalloc(sizeof(ESPSend_t));
	ESPRecv_t * r = pvPortMalloc(sizeof(ESPRecv_t));

	a->id = REFRESH;

	xQueueSend(espserial_send_queue, a, portMAX_DELAY);
	xQueueReceive(espserial_recv_queue, r, portMAX_DELAY);

	vPortFree(a);
	vPortFree(r);

	rtosLOCK_End(lock, espserial_lock);
}

void rtosESPSERIAL_Clear(LockState_t lock){

	rtosLOCK_Begin(lock, espserial_lock);

	ESPSend_t * a = pvPortMalloc(sizeof(ESPSend_t));
	ESPRecv_t * r = pvPortMalloc(sizeof(ESPRecv_t));

	a->id = CLEAR;

	xQueueSend(espserial_send_queue, a, portMAX_DELAY);
	xQueueReceive(espserial_recv_queue, r, portMAX_DELAY);

	vPortFree(a);
	vPortFree(r);

	rtosLOCK_End(lock, espserial_lock);
}


void rtosESPSERIAL_Send(LockState_t lock, void* command, int size, int* bytes_sent){

	rtosLOCK_Begin(lock, espserial_lock);

	ESPSend_t * a = pvPortMalloc(sizeof(ESPSend_t));
	ESPRecv_t * r = pvPortMalloc(sizeof(ESPRecv_t));

	a->id = SEND;
	a->send.command = command;
	a->send.size = size;

	xQueueSend(espserial_send_queue, a, portMAX_DELAY);
	xQueueReceive(espserial_recv_queue, r, portMAX_DELAY);

	*bytes_sent = r->bytes_sent;

	vPortFree(a);
	vPortFree(r);

	rtosLOCK_End(lock, espserial_lock);
}

void rtosESPSERIAL_Recv(LockState_t lock, uint8_t *dst_buffer, int max_bytes, int* bytes_read){

	rtosLOCK_Begin(lock, espserial_lock);

	ESPSend_t * a = pvPortMalloc(sizeof(ESPSend_t));
	ESPRecv_t * r = pvPortMalloc(sizeof(ESPRecv_t));

	a->id = RECV;
	a->recv.dst_buffer = dst_buffer;
	a->recv.max_bytes = max_bytes;

	xQueueSend(espserial_send_queue, a, portMAX_DELAY);
	xQueueReceive(espserial_recv_queue, r, portMAX_DELAY);

	*bytes_read = r->bytes_read;

	vPortFree(a);
	vPortFree(r);

	rtosLOCK_End(lock, espserial_lock);
}

void rtosESPSERIAL_RecvByte(LockState_t lock, uint8_t *dst_buffer, int* bytes_read){
	rtosESPSERIAL_Recv(lock, dst_buffer, 1, bytes_read);
}




void rtosESPSERIAL_ReadLine(LockState_t lock, char ** line){

	if(line == NULL)
		return;

	rtosLOCK_Begin(lock, espserial_lock);

	ESPSend_t * a = pvPortMalloc(sizeof(ESPSend_t));
	ESPRecv_t * r = pvPortMalloc(sizeof(ESPRecv_t));

	a->id = READLINE;
	a->readline.line = line;

	xQueueSend(espserial_send_queue, a, portMAX_DELAY);
	xQueueReceive(espserial_recv_queue, r, portMAX_DELAY);

	vPortFree(a);
	vPortFree(r);


	rtosLOCK_End(lock, espserial_lock);
}

void rtosESPSERIAL_FindStr(LockState_t lock, char * expected_str, char ** found_str){

	if(expected_str == NULL)
		return;

	rtosLOCK_Begin(lock, espserial_lock);

	ESPSend_t * a = pvPortMalloc(sizeof(ESPSend_t));
	ESPRecv_t * r = pvPortMalloc(sizeof(ESPRecv_t));

	a->id = FINDSTR;
	a->findstr.str = expected_str;
	a->findstr.line = found_str;

	xQueueSend(espserial_send_queue, a, portMAX_DELAY);
	xQueueReceive(espserial_recv_queue, r, portMAX_DELAY);

	vPortFree(a);
	vPortFree(r);

	rtosLOCK_End(lock, espserial_lock);
}


void rtosESPSERIAL_WaitResp(LockState_t lock, char *expected_resp, char** resp_obtained, int timeout_ms, ESPState_t* espstate){

	rtosLOCK_Begin(lock, espserial_lock);

	if(*espstate != START){
		rtosLOCK_End(lock, espserial_lock);
		return;
	}

	*espstate = QUEUE;

	ESPSend_t * a = pvPortMalloc(sizeof(ESPSend_t));

	a->id = WAITRESP;
	a->waitresp.timeout_ms = timeout_ms;
	a->waitresp.resp_obtained = resp_obtained;
	a->waitresp.expected_resp = expected_resp;
	a->waitresp.espstate = espstate;

	xQueueSend(espserial_send_queue, a, portMAX_DELAY);

	vPortFree(a);

	rtosLOCK_End(lock, espserial_lock);
}


void rtosESPSERIAL_WaitStr(LockState_t lock, char *expected_resp, char **str_obtained, int timeout_ms, ESPState_t* espstate){

	rtosLOCK_Begin(lock, espserial_lock);

	if(*espstate != START){
		rtosLOCK_End(lock, espserial_lock);
		return;
	}

	*espstate = QUEUE;

	ESPSend_t * a = pvPortMalloc(sizeof(ESPSend_t));

	a->id = WAITSTR;
	a->waitstr.timeout_ms = timeout_ms;
	a->waitstr.str_obtained = str_obtained;
	a->waitstr.expected_resp = expected_resp;
	a->waitstr.espstate = espstate;

	xQueueSend(espserial_send_queue, a, portMAX_DELAY);

	vPortFree(a);

	rtosLOCK_End(lock, espserial_lock);
}









void rtosESPSERIAL_SendAT(LockState_t lock, char * command, char * expected_resp, int timeout_ms, ESPState_t* espstate){

	rtosLOCK_Begin(lock, espserial_lock);

	if(*espstate != START){
		rtosLOCK_End(lock, espserial_lock);
		return;
	}
	*espstate = QUEUE;

	ESPSend_t * a = pvPortMalloc(sizeof(ESPSend_t));

	a->id = AT;
	a->at.command = command;
	a->at.expected_resp = expected_resp;
	a->at.timeout_ms = timeout_ms;
	a->at.espstate = espstate;

	xQueueSend(espserial_send_queue, a, portMAX_DELAY);

	vPortFree(a);

	rtosLOCK_End(lock, espserial_lock);

}





static bool rtosESPSERIAL_xParseStatus(char * status_msg){
	bool ret = false;
	int status_msg_length = strlen(status_msg);
	char * found_status_msg;
	if((found_status_msg = strstr((char *)ESPSERIALBuffer, status_msg)) != NULL){
			ret = true;
			memmove(found_status_msg, (char *)(found_status_msg + status_msg_length), tail);
			tail -= status_msg_length;
			ESPSERIALBuffer[tail] = 0;
	}
	return ret;
}

static void rtosESPSERIAL_xHandleStatus(){

	rtosESPSERIAL_xParseStatus("WIFI CONNECTED");
	rtosESPSERIAL_xParseStatus("WIFI GOT IP");
	rtosESPSERIAL_xParseStatus("WIFI DISCONNECT");
	rtosESPSERIAL_xParseStatus("busy s...");
	rtosESPSERIAL_xParseStatus("busy p...");

}

void rtosESPSERIAL_xRefresh(){
	TickType_t refresh_start_time;

	TickType_t start_time = rtosTIMEOUT_Start();
	TickType_t timeout = pdMS_TO_TICKS(RTOS_ESPSERIAL_BUFFERREFRESH_TIMEOUT);

	while(1){
		tail += ESPSERIAL_Recv(&(ESPSERIALBuffer[tail]), RTOS_ESPSERIAL_BUFFER_SIZE - 1 - tail);
		ESPSERIALBuffer[tail] = 0;
		rtosESPSERIAL_xHandleStatus();
		available_bytes = tail - head;
		if(rtosTIMEOUT_Expired(start_time, timeout))
			break;
		refresh_start_time = xTaskGetTickCount();
		vTaskDelayUntil(&refresh_start_time, refresh_rate_tick);	}
}

void rtosESPSERIAL_xClear(){
	ESPSERIAL_Recv(ESPSERIALBuffer, RTOS_ESPSERIAL_BUFFER_SIZE);
	memset(ESPSERIALBuffer, 0, RTOS_ESPSERIAL_BUFFER_SIZE);
	available_bytes = tail = head = 0;
}

void rtosESPSERIAL_xSend(void * buf, int size, int *bytes_sent){
	rtosESPSERIAL_xClear();
	*bytes_sent = ESPSERIAL_Send(buf, size);
	if(RTOS_ESPSERIAL_PRINTF_DEBUG){
		for(int i = 0; i < size; i++)
		printf("%c", ((char *)buf)[i]);
	}
}

void rtosESPSERIAL_xRecv(uint8_t* buffer, int bytes_to_read, int* bytes_read){
	if(available_bytes){
		bytes_to_read = (available_bytes >= bytes_to_read) ? bytes_to_read : available_bytes;
		memmove(buffer, &(ESPSERIALBuffer[head]), bytes_to_read);
		if(RTOS_ESPSERIAL_PRINTF_DEBUG){
			for(int i = 0; i < bytes_to_read; i++){
					printf("%c", ESPSERIALBuffer[head + i]);
			}
		}
		head += *bytes_read = bytes_to_read;
		available_bytes = tail - head;
	}
	else *bytes_read = 0;
}



void rtosESPSERIAL_xReadLine(char ** line){

	*(line) = NULL;

	if((line == NULL) || (available_bytes < 2))
		return;

	char * marker = NULL;

	if ((marker = strstr((char *)&ESPSERIALBuffer[head], "\r\n")) != NULL) {
		marker[0] = 0;
		*line = (char *) &(ESPSERIALBuffer[head]);
		head = (marker - (char *)ESPSERIALBuffer) + 2;
	}

	if(RTOS_ESPSERIAL_PRINTF_DEBUG && *line != NULL){
		printf("%s\r\n", (char *) *line);
	}

	available_bytes = tail - head;

}

void rtosESPSERIAL_xFindStr(char * str_to_find, char ** line){

	*line = NULL;

	if(line == NULL || available_bytes < 2)
		return;

	int str_size = strlen(str_to_find);

	char * marker = NULL;

	if ((marker = strstr((char *)&ESPSERIALBuffer[head], str_to_find)) != NULL) {
		*line = marker;
		head = (marker - (char *)ESPSERIALBuffer + str_size);
	}
	if(RTOS_ESPSERIAL_PRINTF_DEBUG && *line != NULL)
		printf("%s\r\n", (char *) *line);

	available_bytes = tail - head;

}

void rtosESPSERIAL_xWaitResp(char *expected_resp, char** resp_obtained, int timeout_ms, ESPState_t* espstate){

	char * line = NULL;

	TickType_t refresh_start_time;

	TickType_t start_time = rtosTIMEOUT_Start();
	TickType_t timeout = pdMS_TO_TICKS(timeout_ms);

	*espstate = WAIT;

	bool loop = true;
	int state = 0;

	while (loop) {
		switch (state){
		case 0:
			rtosESPSERIAL_xRefresh();
			state++;
			break;

		case 1:
			rtosESPSERIAL_xReadLine(&line);
			if(line != NULL){
				if(strstr(line, expected_resp) != NULL){
					*espstate = SUCCESS;
					if(resp_obtained != NULL)
						*resp_obtained = line;
					loop = false;
				}
				else if((strstr(line, "ERROR") != NULL) || (strstr(line, "FAIL") != NULL)){
					loop = false;
					*espstate = FAIL;
				}
			}
			else
				state = 0;

			break;
		}
		if(rtosTIMEOUT_Expired(start_time, timeout)){
			loop = false;
			*espstate = FAIL;
		}
		refresh_start_time = xTaskGetTickCount();
		vTaskDelayUntil(&refresh_start_time, refresh_rate_tick);
	}
}

void rtosESPSERIAL_xWaitStr(char *expected_resp, char **str_obtained, int timeout_ms, ESPState_t* espstate){

	TickType_t refresh_start_time;
	TickType_t start_time = rtosTIMEOUT_Start();
	TickType_t timeout = pdMS_TO_TICKS(timeout_ms);
	*espstate = WAIT;

	char * line = NULL;

	int state = 0;

	bool loop = true;
	while (loop) {
		switch (state){
		case 0:
			rtosESPSERIAL_xRefresh();
			state++;
			break;

		case 1:
			rtosESPSERIAL_xFindStr(expected_resp, &line);
			if(line != NULL){
				*espstate = SUCCESS;
				if(str_obtained != NULL)
					*str_obtained = line;
				loop = false;
			}
			else
				state = 0;
			break;
		}
		if(rtosTIMEOUT_Expired(start_time, timeout)){
			loop = false;
			*espstate = FAIL;
		}
		refresh_start_time = xTaskGetTickCount();
		vTaskDelayUntil(&refresh_start_time, refresh_rate_tick);
	}
}

void rtosESPSERIAL_xSendAT(char * command, char * expected_resp, int timeout_ms, ESPState_t* espstate){

	if(command == NULL || expected_resp == NULL)
		return;

	TickType_t refresh_start_time;

	char * at = pvPortMalloc(strlen(command) + 3);
	int bytes_to_send = sprintf(at, "%s\r\n", command);
	int bytes_sent = 0;

	bool loop = true;
	int state = 0;
	while(loop){
		switch (state){
		case 0:
			rtosESPSERIAL_xSend((void *) at, bytes_to_send, &bytes_sent);
			if(bytes_sent == bytes_to_send)
				state++;
			else
				loop = false;
			break;

		case 1:
			rtosESPSERIAL_xWaitResp(expected_resp, NULL, timeout_ms, espstate);
			loop = false;
			break;
		}
		refresh_start_time = xTaskGetTickCount();
		vTaskDelayUntil(&refresh_start_time, refresh_rate_tick);
	}

	vPortFree(at);
}

bool rtosESPSERIAL_Setup(bool restore){
	rtosESPSERIAL_LockBegin();

	TickType_t start_time = rtosTIMEOUT_Start();
	TickType_t timeout = pdMS_TO_TICKS(RTOS_ESPSERIAL_SETUP_TIMEOUT);
	TickType_t refresh_start_time;

	ESPState_t espstate = START;
	int state = 0;
	bool loop = true;
	while (loop) {
		switch (state){
			case 0:
				rtosESPSERIAL_SendAT(LOCK_CONTINUE, "AT", "OK", RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);
				if(espstate == SUCCESS){
					espstate = START;
					state++;
				}
				else if(espstate == FAIL)
					espstate = START;
				break;

			case 1:
				if(restore)
					rtosESPSERIAL_SendAT(LOCK_CONTINUE, "AT+RESTORE", "OK", RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);
				else
					rtosESPSERIAL_SendAT(LOCK_CONTINUE, "AT+RST", "OK", RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);
				if(espstate == SUCCESS){
					espstate = START;
					state++;
				}
				else if(espstate == FAIL)
					espstate = START;
				break;

			case 2:
				rtosESPSERIAL_SendAT(LOCK_CONTINUE, "ATE0", "OK", RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);
				if(espstate == SUCCESS){
					state++;
					espstate = START;
				}
				else if(espstate == FAIL)
					espstate = START;
				break;

			case 3:
				rtosESPSERIAL_SendAT(LOCK_CONTINUE, "AT+CWMODE=1", "OK", RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);
				if(espstate == SUCCESS){
					espstate = START;
					state++;
				}
				else if(espstate == FAIL)
					espstate = START;
				break;

			case 4:
				rtosESPSERIAL_SendAT(LOCK_CONTINUE, "AT+CWLAPOPT=1,127", "OK", RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);
				if(espstate == SUCCESS){
					espstate = START;
					state++;
				}
				else if(espstate == FAIL)
					espstate = START;
				break;

			case 5:
				rtosESPSERIAL_SendAT(LOCK_CONTINUE, "AT+CIPMUX=0", "OK", RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);
				if(espstate == SUCCESS){
					rtosESPSERIAL_LockEnd();
					return true;
				}
				else if(espstate == FAIL)
					espstate = START;
			break;

		}
		refresh_start_time = xTaskGetTickCount();
		vTaskDelayUntil(&refresh_start_time, refresh_rate_tick);

		if(rtosTIMEOUT_Expired(start_time, timeout))
			loop = false;
	}

	return false;

}

void rtosESPSERIAL_SetupTask(void * a){
	rtosESPSERIAL_Setup(RTOS_ESPSERIAL_RESTORE_ON_DEFAULT_SETUP);
	vTaskDelete(xTaskGetCurrentTaskHandle());
}




static void rtosESPSERIAL_Task(void *pvParameters){
	ESPSERIAL_Init(baud);
	ESPSend_t instruction;
	ESPRecv_t recv;
	while(1){
		if(xQueueReceive(espserial_send_queue, &instruction, portMAX_DELAY) == pdPASS);
		switch(instruction.id){
			case REFRESH:
				rtosESPSERIAL_xRefresh();
				xQueueSend(espserial_recv_queue, &recv, portMAX_DELAY);
				break;

			case SEND:
				rtosESPSERIAL_xSend(instruction.send.command,
									instruction.send.size,
									&(recv.bytes_sent));
				xQueueSend(espserial_recv_queue, &recv, portMAX_DELAY);
				break;

			case RECV:
				rtosESPSERIAL_xRecv(instruction.recv.dst_buffer,
									instruction.recv.max_bytes,
									&(recv.bytes_read));
				xQueueSend(espserial_recv_queue, &recv, portMAX_DELAY);
				break;

			case CLEAR:
				rtosESPSERIAL_xClear();
				xQueueSend(espserial_recv_queue, &recv, portMAX_DELAY);
				break;

			case READLINE:
				rtosESPSERIAL_xReadLine(instruction.readline.line);
				xQueueSend(espserial_recv_queue, &recv, portMAX_DELAY);
				break;

			case FINDSTR:
				rtosESPSERIAL_xFindStr(instruction.findstr.str,
									   instruction.findstr.line);
				xQueueSend(espserial_recv_queue, &recv, portMAX_DELAY);

				break;

			case WAITRESP:
				rtosESPSERIAL_xWaitResp(instruction.waitresp.expected_resp,
										instruction.waitresp.resp_obtained,
										instruction.waitresp.timeout_ms,
										instruction.waitresp.espstate);
				xQueueSend(espserial_recv_queue, &recv, portMAX_DELAY);

				break;

			case WAITSTR:
				rtosESPSERIAL_xWaitStr(instruction.waitstr.expected_resp,
									   instruction.waitstr.str_obtained,
									   instruction.waitstr.timeout_ms,
									   instruction.waitstr.espstate);
				break;

			case AT:
				rtosESPSERIAL_xSendAT(instruction.at.command,
									  instruction.at.expected_resp,
									  instruction.at.timeout_ms,
									  instruction.at.espstate);
				break;
		}

	}

}

