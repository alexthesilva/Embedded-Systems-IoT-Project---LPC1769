#include "netwifi.h"
#include <FreeRTOS.h>
#include <stdbool.h>
#include "rtosespserial.h"
#include <stdio.h>
#include <task.h>
#include <timers.h>
#include <string.h>
#include "rtostimeout.h"


typedef struct {
	uint8_t id;
	union{
		struct {
			char* ssid;
			char* pass;
			bool makedefault;
		} connect;


		struct {
			bool* res;
		} check;

		struct {
			int max_ap;
			int* scanned;
		} scan;

		struct {
			int index;
		} scanned;

	};
} WIFISend_t;


typedef struct {
	uint8_t id;
	union{
		struct {
			bool res;
		} connect;

		struct {
			bool res;
		} disconnect;

		struct {
			Network_t* network;
		} curr;

		struct {
			Network_t* scanned_network;
		} scanned;
	};
} WIFIRecv_t;

void netWIFI_Task(void* arg);

enum {CONNECT, DISCONNECT, CURR, CHECK, SCAN, SCANNED};

static bool started = false;
static Network_t** scanned_networks  = NULL;
static int net_number = 0;

static QueueHandle_t wifi_send_queue = NULL;
static QueueHandle_t wifi_recv_queue = NULL;

static bool init = false;



bool netWIFI_Init(){
	if(init)
		return true;

	if((wifi_send_queue = xQueueCreate(NETWORK_WIFI_QUEUES_SIZE, sizeof(WIFISend_t))) == NULL)
		return false;

	if((wifi_recv_queue = xQueueCreate(NETWORK_WIFI_QUEUES_SIZE, sizeof(WIFIRecv_t))) == NULL)
		return false;

	if(xTaskCreate(netWIFI_Task, "WIFI_Task", NETWORK_WIFI_STACK_SIZE, NULL, 1, NULL) != pdPASS)
		return false;

	init = true;
	return true;

}

bool netWIFI_Connect(char * ssid, char *pass, bool makedefault){
	WIFISend_t * a = pvPortMalloc(sizeof(WIFISend_t));
	WIFIRecv_t * r = pvPortMalloc(sizeof(WIFIRecv_t));

	a->id = CONNECT;
	a->connect.ssid = ssid;
	a->connect.pass = pass;
	a->connect.makedefault = makedefault;

	xQueueSend(wifi_send_queue, a, portMAX_DELAY);
	xQueueReceive(wifi_recv_queue, r, portMAX_DELAY);
	bool ret = r->connect.res;

	vPortFree(a);
	vPortFree(r);

	return ret;
}

bool netWIFI_Disconnect(){
	WIFISend_t * a = pvPortMalloc(sizeof(WIFISend_t));
	WIFIRecv_t * r = pvPortMalloc(sizeof(WIFIRecv_t));

	a->id = DISCONNECT;

	xQueueSend(wifi_send_queue, a, portMAX_DELAY);
	xQueueReceive(wifi_recv_queue, r, portMAX_DELAY);

	bool ret = r->disconnect.res;

	vPortFree(a);
	vPortFree(r);

	return ret;
}

Network_t* netWIFI_CurrentConn(){
	WIFISend_t * a = pvPortMalloc(sizeof(WIFISend_t));
	WIFIRecv_t * r = pvPortMalloc(sizeof(WIFIRecv_t));

	a->id = CURR;

	xQueueSend(wifi_send_queue, a, portMAX_DELAY);
	xQueueReceive(wifi_recv_queue, r, portMAX_DELAY);

	Network_t* ret = r->curr.network;

	vPortFree(a);
	vPortFree(r);

	return ret;

}

void netWIFI_Check(bool* result){
	WIFISend_t * a = pvPortMalloc(sizeof(WIFISend_t));
	WIFIRecv_t * r = pvPortMalloc(sizeof(WIFIRecv_t));

	a->id = CHECK;

	a->check.res = result;

	xQueueSend(wifi_send_queue, a, portMAX_DELAY);
	xQueueReceive(wifi_recv_queue, r, portMAX_DELAY);

	vPortFree(a);
	vPortFree(r);

}

void netWIFI_Scan(int max_ap, int* nets_scanned){
	WIFISend_t * a = pvPortMalloc(sizeof(WIFISend_t));
	WIFIRecv_t * r = pvPortMalloc(sizeof(WIFIRecv_t));

	a->id = SCAN;
	a->scan.max_ap = max_ap;
	a->scan.scanned = nets_scanned;

	xQueueSend(wifi_send_queue, a, portMAX_DELAY);
	xQueueReceive(wifi_recv_queue, r, portMAX_DELAY);

	vPortFree(a);
	vPortFree(r);

}

Network_t* netWIFI_ScannedSSID(int index){
	WIFISend_t * a = pvPortMalloc(sizeof(WIFISend_t));
	WIFIRecv_t * r = pvPortMalloc(sizeof(WIFIRecv_t));

	a->id = SCANNED;
	a->scanned.index = index;

	xQueueSend(wifi_send_queue, a, portMAX_DELAY);
	xQueueReceive(wifi_recv_queue, r, portMAX_DELAY);

	Network_t* ret = r->scanned.scanned_network;

	vPortFree(a);
	vPortFree(r);

	return ret;
}

bool netWIFI_xConnect(char * ssid, char *pass, bool makedefault){

	char *at = pvPortMalloc(16 + NETWORK_MAX_SSID_LENGTH + NETWORK_MAX_PASSWORD_LENGTH);
	sprintf(at, "AT+CWJAP_%s=\"%s\",\"%s\"", (makedefault) ? "DEF" : "CUR", ssid, pass);
	ESPState_t espstate = START;
	rtosESPSERIAL_LockBegin();
	while(1){
		rtosESPSERIAL_SendAT(LOCK_CONTINUE, at, "OK", NETWORK_CONNECT_TIMEOUT, &espstate);
		if(espstate == SUCCESS || espstate == FAIL)
			break;
	}
	vPortFree(at);

	rtosESPSERIAL_LockEnd();

	return (espstate == SUCCESS) ? true : false;

}

bool netWIFI_xDisconnect(){

	ESPState_t espstate = START;
	rtosESPSERIAL_LockBegin();
	while(1){
		rtosESPSERIAL_SendAT(LOCK_CONTINUE, "AT+CWQAP", "OK", RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);
		if(espstate == SUCCESS || espstate == FAIL)
			break;
	}

	rtosESPSERIAL_LockEnd();

	return (espstate == SUCCESS) ? true : false;
}

void netWIFI_xCheck(bool* result){

	int status = 5;
	char * status_str = NULL;
	ESPState_t espstate = START;
	int bytes_sent = 0;
	char* at = "AT+CIPSTATUS\r\n";
	int at_size = strlen(at);
	rtosESPSERIAL_LockBegin();
	int state = 0;
	bool loop = true;
	while(loop){
		switch (state){
		case 0:
			rtosESPSERIAL_Send(LOCK_CONTINUE, at, at_size, &bytes_sent);
			if(bytes_sent == at_size){
				state++;
			}
			else
				loop = false;
			break;

		case 1:
			rtosESPSERIAL_WaitStr(LOCK_CONTINUE,  "STATUS:", &status_str, RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);
			if(espstate == SUCCESS){
				espstate = START;
				state++;
			}
			else if(espstate == FAIL)
				loop = false;
			break;
		case 2:
			sscanf(status_str, "STATUS:%d", &status);
			loop = false;
			break;
		}
	}
	*result = (status == 5) ? false : true;

	rtosESPSERIAL_LockEnd();
}


void netWIFI_xScan(int max_ap, int* nets_scanned){
	rtosESPSERIAL_LockBegin();

	if(started){
		for(int i = 0; i < net_number; i++){
			vPortFree((scanned_networks[i])->mac);
			vPortFree((scanned_networks[i])->ssid);
			vPortFree(&(scanned_networks[i]));
		}
		vPortFree(scanned_networks);
	}
	else
		started = true;

	net_number = 0;

	int sent_bytes = 0;
	char cmd[] = "AT+CWLAP\r\n";
	char * curr_line;
	int res;
	scanned_networks = pvPortMalloc(sizeof(Network_t*) * max_ap);

	Network_t* test = pvPortMalloc(sizeof(Network_t));
	test->mac = pvPortMalloc(NETWORK_MAC_ADDRESS_LENGTH);
	test->ssid = pvPortMalloc(NETWORK_MAX_SSID_LENGTH);

	TickType_t start_time = rtosTIMEOUT_Start();
	TickType_t timeout = pdMS_TO_TICKS(NETWORK_WIFI_SCAN_TIMEOUT);

	bool loop = true;
	int state = 0;
	while (loop) {
		switch(state){
		case 0:
			rtosESPSERIAL_Send(LOCK_CONTINUE, cmd, strlen(cmd), &sent_bytes);
			if(sent_bytes == strlen(cmd))
				state++;
			else
				loop = false;
			break;
		case 1:
			rtosESPSERIAL_Refresh(LOCK_CONTINUE);
			state++;
			break;
		case 2:
			if(net_number == max_ap - 1)
				loop = false;
			rtosESPSERIAL_ReadLine(LOCK_CONTINUE, &curr_line);

			if(curr_line != NULL){
				if(strstr(curr_line, "+CWLAP:") != NULL){

					res = sscanf(curr_line, "+CWLAP:(%d,\"%[^\"]\",%d,\"%[^\"]\",%d,%d,%d)", &(test->ecn),
																			  (test->ssid),
																			 &(test->rssi),
																			  (test->mac),
																			 &(test->channel),
																			 &(test->freq_offset),
																			 &(test->freq_calibration));
					//res = sscanf(curr_line, "+CWLAP:(\"%[^\"]\")", (test->ssid));
					if(res == 7){
						scanned_networks[net_number] = pvPortMalloc(sizeof(Network_t));
						scanned_networks[net_number]->ssid = pvPortMalloc(strlen(test->ssid) + 1);
						scanned_networks[net_number]->mac = pvPortMalloc(strlen(test->mac) + 1);
						res = sscanf(curr_line, "+CWLAP:(%d,\"%[^\"]\",%d,\"%[^\"]\",%d,%d,%d)", &(scanned_networks[net_number]->ecn),
																								  (scanned_networks[net_number]->ssid),
																								 &(scanned_networks[net_number]->rssi),
																								  (scanned_networks[net_number]->mac),
																								 &(scanned_networks[net_number]->channel),
																								 &(scanned_networks[net_number]->freq_offset),
																								 &(scanned_networks[net_number]->freq_calibration));
						//res = sscanf(curr_line, "+CWLAP:(\"%[^\"]\")", (scanned_networks[net_number]->ssid));
						net_number++;

					};
				}
				else if(strstr(curr_line, "OK") != NULL)
					loop = false;
			}
			else
				state--;
		}
		if(rtosTIMEOUT_Expired(start_time, timeout))
			loop = false;
	}

	vPortFree(test->mac);
	vPortFree(test->ssid);
	vPortFree(test);
	*nets_scanned = net_number;

	rtosESPSERIAL_LockEnd();

}

Network_t* netWIFI_xScannedSSID(int index){
	if(started && (index >= 0) && (index < net_number)){
		return scanned_networks[index];
	}
	return NULL;
}

void netWIFI_Task(void* arg){
	WIFISend_t instruction;
	WIFIRecv_t recv;
	while(1){
		if(xQueueReceive(wifi_send_queue, &instruction, portMAX_DELAY) == pdPASS){
			switch(instruction.id){
			case CONNECT:
				recv.connect.res = netWIFI_xConnect(instruction.connect.ssid, instruction.connect.pass, instruction.connect.makedefault);
				xQueueSend(wifi_recv_queue, &recv, portMAX_DELAY);
				break;
			case DISCONNECT:
				recv.disconnect.res = netWIFI_xDisconnect();
				xQueueSend(wifi_recv_queue, &recv, portMAX_DELAY);
				break;
			case CHECK:
				netWIFI_xCheck(instruction.check.res);
				xQueueSend(wifi_recv_queue, &recv, portMAX_DELAY);
				break;
			case SCAN:
				netWIFI_xScan(instruction.scan.max_ap, instruction.scan.scanned);
				xQueueSend(wifi_recv_queue, &recv, portMAX_DELAY);
				break;
			case SCANNED:
				recv.scanned.scanned_network = netWIFI_xScannedSSID(instruction.scanned.index);
				xQueueSend(wifi_recv_queue, &recv, portMAX_DELAY);
				break;
			}
		}
	}




}
