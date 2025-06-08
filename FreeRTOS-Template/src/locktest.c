/*
#include <FreeRTOS.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <string.h>
#include <time.h>

#include "rtosrtc.h"
#include "rtoslcd.h"
#include "rtospir.h"
#include "rtosled.h"
#include "rtosbh1750.h"
#include "rtosencoder.h"
#include "rtosespserial.h"
#include "netwifi.h"
#include "netntp.h"
#include "uimenu.h"

#include "netmqtt.h"
#include "espserial.h"

struct tm relogio;
struct tm dateTime;
void RTOS_Init(){
	relogio.tm_hour = 0;
	relogio.tm_min =  0;
	relogio.tm_sec =  0;
	relogio.tm_mday = 1;
	relogio.tm_mon = 1;
	relogio.tm_year = 1900;
	relogio.tm_wday = 0;

	assert(rtosRTC_Init(&relogio) == true);
	assert(rtosLED_Init(false) == true);
	assert(rtosLCDText_Init() == true);
	assert(rtosENCODER_Init() == true);
	assert(rtosPIR_Init(1000) == true);
	assert(rtosBH1750_Init() == true);
	assert(rtosESPSERIAL_Init(115200) == true);
	assert(rtosENCODER_Init() == true);
}

void NETWORK_Init(){
	assert(netMQTT_Init() == true);
	assert(netNTP_Init() == true);
}


void task_1(void * a){

	while ( 1 ) {
		rtosLCDText_LockBegin();
		rtosLCDText_Locate(LOCK_CONTINUE, 1, 0);
		rtosLCDText_WriteString(LOCK_CONTINUE, "aaa");
		rtosLCDText_LockEnd();

	}
}


void task_2(void * a){
	while(1){
		rtosLCDText_LockBegin();
		rtosLCDText_Locate(LOCK_CONTINUE, 0, 9);
		rtosLCDText_WriteString(LOCK_CONTINUE, "bbb");
		rtosLCDText_LockEnd();

	};
}


void task_3(void * a){
	while(1){
		rtosLCDText_LockBegin();
		rtosLCDText_Locate(LOCK_CONTINUE, 0, 0);
		rtosLCDText_WriteString(LOCK_CONTINUE, "ccc");
		rtosLCDText_LockEnd();
	};
}



int main(void) {

	RTOS_Init();
	NETWORK_Init();
	uiMENU_Init();

	assert(xTaskCreate(task_1, "task_1", configMINIMAL_STACK_SIZE * 3, NULL, 1, NULL) ==  pdPASS);
	assert(xTaskCreate(task_2, "task_2", configMINIMAL_STACK_SIZE * 3, NULL, 1, NULL) == pdPASS);
	assert(xTaskCreate(task_3, "task_3", configMINIMAL_STACK_SIZE * 3, NULL, 1, NULL) == pdPASS);

	vTaskStartScheduler();

	while(1);

	return 0;
}




*/
