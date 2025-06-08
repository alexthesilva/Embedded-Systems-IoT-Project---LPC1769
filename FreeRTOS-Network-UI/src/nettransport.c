#include "nettransport.h"
#include <FreeRTOS.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <task.h>
#include <queue.h>
#include "MQTTPacket.h"
#include "transport.h"
#include "rtosespserial.h"
#include "netwifi.h"
#include "rtoslock.h"
#include "netmqtt.h"
#include "rtostimeout.h"

bool netUDP_Start(const char * host, int port){


	char * at = pvPortMalloc(128);
	sprintf(at, "AT+CIPSTART=\"UDP\",\"%s\",%d", host, port);
	ESPState_t espstate = START;
	rtosESPSERIAL_LockBegin();
	while(1){
		rtosESPSERIAL_SendAT(LOCK_CONTINUE, at, "OK", RTOS_ESPSERIAL_LONG_PAUSE, &espstate);
		if(espstate == SUCCESS || espstate == FAIL)
			break;
	}
	vPortFree(at);

	return (espstate == SUCCESS) ? true : false;
}

bool netTCP_Start(const char *host, const unsigned short int port,
		const unsigned short int keepalive) {


	char * at = pvPortMalloc(128);
	sprintf(at, "AT+CIPSTART=\"TCP\",\"%s\",%d,%d", host, port, keepalive);
	ESPState_t espstate = START;
	rtosESPSERIAL_LockBegin();
	while(1){
		rtosESPSERIAL_SendAT(LOCK_CONTINUE, at, "OK", RTOS_ESPSERIAL_LONG_PAUSE, &espstate);
		if(espstate == SUCCESS || espstate == FAIL)
			break;
	}
	vPortFree(at);

	return (espstate == SUCCESS) ? true : false;

}

bool netUDP_Close(){
	ESPState_t espstate = START;
	while(1){
		rtosESPSERIAL_SendAT(LOCK_CONTINUE, "AT+CIPCLOSE", "OK", RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);
		if(espstate == SUCCESS || espstate == FAIL)
			break;

	}
	rtosESPSERIAL_LockEnd();

	return (espstate == SUCCESS) ? true : false;
}

bool netTCP_Close(){
	return netUDP_Close();
}


int netTRANSPORT_Send(unsigned char *address, unsigned int bytes) {

	int ret = 0;

	char * at = pvPortMalloc(32);
	sprintf(at, "AT+CIPSEND=%u", bytes);

	ESPState_t espstate = START;
	int bytes_sent = 0;

	TickType_t start_time = rtosTIMEOUT_Start();
	TickType_t timeout = pdMS_TO_TICKS(NETWORK_SEND_TIMEOUT);

	int state = 0;
	bool loop = true;
	while(loop){
		switch (state){
		case 0:
			rtosESPSERIAL_SendAT(LOCK_CONTINUE, at, "OK", RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);//rtosESPSERIAL_xSendAT(at, "OK", SHORT_PAUSE, &espstate);//
			if(espstate == SUCCESS){
				espstate = START;
				state++;
			}
			else if(espstate == FAIL)
				loop = false;
			break;

		case 1:
			rtosESPSERIAL_WaitStr(LOCK_CONTINUE,  "> ", NULL, RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);//rtosESPSERIAL_xWaitStr( "> ", NULL, SHORT_PAUSE, &espstate);//
			if(espstate == SUCCESS){
				espstate = START;
				state++;
			}
			else if(espstate == FAIL)
				loop = false;
			break;

		case 2:
			rtosESPSERIAL_Send(LOCK_CONTINUE, (void *)address, bytes, &bytes_sent);//rtosESPSERIAL_xSend((void *)address, bytes, &bytes_sent);
			if(bytes == bytes_sent)
				state++;
			else
				loop = false;
			break;

		case 3:
			rtosESPSERIAL_WaitStr(LOCK_CONTINUE, "SEND OK", NULL, RTOS_ESPSERIAL_SHORT_PAUSE, &espstate);//rtosESPSERIAL_xWaitStr("SEND OK", NULL, SHORT_PAUSE, &espstate);//rtosESPSERIAL_WaitStr(LOCK_CONTINUE, "SEND OK", NULL, SHORT_PAUSE, &espstate);
			if(espstate == SUCCESS){
				espstate = START;
				state++;
			}
			else if(espstate == FAIL)
				loop = false;
			break;

		case 4:
			ret = bytes;
			loop = false;
			break;
		}
		if(rtosTIMEOUT_Expired(start_time, timeout)){
			loop = false;
			ret = -1;
		}
	}

	vPortFree(at);

	return ret;
}


int netTRANSPORT_Recv(unsigned char *address, unsigned int max_bytes) {

	int ret = 0;

	static int packet_length;
	static int index;
	int bytes_read = 0;

	bool loop = true;
	char * curr_line = NULL;

	TickType_t start_time = rtosTIMEOUT_Start();
	TickType_t timeout = pdMS_TO_TICKS(NETWORK_RECEIVE_TIMEOUT);

	int state = (packet_length > 0) ? 2 : 0;


	while(loop){

		switch(state){
		case 0:
			rtosESPSERIAL_Refresh(LOCK_CONTINUE);
			rtosESPSERIAL_FindStr(LOCK_CONTINUE, "+IPD,", &curr_line);

			if(curr_line != NULL){
				state++;
			}
			break;

		case 1:
			sscanf(curr_line, "+IPD,%d:", &packet_length);
			rtosESPSERIAL_FindStr(LOCK_CONTINUE, ":", &curr_line);
			index = 0;
			state++;
			break;

		case 2:
			rtosESPSERIAL_Recv(LOCK_CONTINUE, address, (max_bytes <= packet_length) ? max_bytes : packet_length, &bytes_read);
			ret += bytes_read;
			index += bytes_read;
			packet_length -= bytes_read;
			if((ret == max_bytes) || (packet_length == 0))
				loop = false;
			break;
		}

		if(rtosTIMEOUT_Expired(start_time, timeout)){
			ret = -1;
			loop = false;
			index = 0;
			packet_length = 0;
		}

	}
	return ret;
}
