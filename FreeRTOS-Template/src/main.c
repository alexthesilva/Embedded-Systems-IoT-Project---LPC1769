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
#include "timers.h"
#include "menus.h"
#include "main.h"



static TaskHandle_t normal_task = NULL;
static TaskHandle_t maintenance_task = NULL;

static TimerHandle_t manage_light_timer = NULL;
static TimerHandle_t mqtt_timer = NULL;

static int mqtt_publish_interval_s = MQTT_PUBLISH_INTERVAL_S;

static int light = 200;
static int sensorlight;
static int last_sensorlight;
static bool presence;

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
	netMQTT_Config(DEFAULT_MQTT_ADDRESS, DEFAULT_MQTT_DEVICE_TOKEN, DEFAULT_MQTT_PORT, DEFAULT_CONNECTION_KEEPALIVE_S);
	assert(netNTP_Init() == true);
	assert(netWIFI_Init() == true);

}

void APPLICATION_Init(){
	xTaskCreate(appMENU_NormalMode, "Normal_Mode", configMINIMAL_STACK_SIZE * 3, NULL, 1, &normal_task);
	xTaskCreate(appMENU_MaintenanceMode, "Maintenance_Mode", configMINIMAL_STACK_SIZE * 7, NULL, 1, &maintenance_task);
}



void appMENU_NormalAffixHandler(void* a){
	rtosLCDText_Clear(LOCK_BEGIN);
	last_sensorlight = (rtosBH1750_Ready(true)) ? (int) rtosBH1750_GetLight() : -1;
	TickType_t start_time = xTaskGetTickCount();
	while(xTaskGetTickCount() - start_time < pdMS_TO_TICKS(5000)){
		rtosRTC_GetValue ( &dateTime );
		rtosLCDText_Locate (LOCK_CONTINUE, 0, 0 );
		rtosLCDText_Printf (LOCK_CONTINUE, "%02d:%02d:%02d",dateTime.tm_hour,dateTime.tm_min,dateTime.tm_sec);
		rtosLCDText_Locate (LOCK_CONTINUE, 0, 9);
		if(last_sensorlight > -1)
			rtosLCDText_Printf (LOCK_CONTINUE, "L:%05d", (rtosBH1750_Ready(true)) ? (int) rtosBH1750_GetLight() : last_sensorlight);
		else
			rtosLCDText_Printf (LOCK_CONTINUE, "L:ERR");
		rtosLCDText_Locate (LOCK_CONTINUE, 1 , 0 );
		rtosLCDText_Printf (LOCK_CONTINUE, "%04d-%02d-%02d   %s",dateTime.tm_year,dateTime.tm_mon,dateTime.tm_mday, appMENU_GetWeekDay(dateTime.tm_wday));
	}
	rtosLCDText_Clear(LOCK_END);
}

enum { LIGHT_IDLE, LIGHT_ENTRY, LIGHT };

void appMENU_NormalManageLightISR(TimerHandle_t xTimer){
	static int state;
	presence = rtosPIR_GetValue();
	switch (state){
	case LIGHT_IDLE:
		if(presence){
			if(rtosBH1750_Ready(true))
				sensorlight = rtosBH1750_GetLight();
			if(sensorlight<light)
				state = LIGHT_ENTRY;
		}
		break;

	case LIGHT_ENTRY:
		rtosLED_On();
		state = LIGHT;
		break;

	case LIGHT:
		if(!presence){
			rtosLED_Off();
			state = LIGHT_IDLE;
		}
		break;
	}
}

void appMENU_NormalPublishISR(TimerHandle_t xTimer){
	if(netMQTT_Ready()){
		char* presence_str = pvPortMalloc(2);
		sprintf(presence_str, "%d", presence);
		char* light_str = pvPortMalloc(7);
		sprintf(light_str, "%d", sensorlight);
		netMQTT_Publish("presence", presence_str);
		netMQTT_Publish("light", light_str);
		vPortFree(presence_str);
		vPortFree(light_str);
	}
	else
		netMQTT_Start();

}


void appMENU_NormalMode(void * arg){
	if(DEFAULT_WIFI_CONNECT)
		netWIFI_Connect(DEFAULT_SSID, DEFAULT_PASS, true);

	manage_light_timer = xTimerCreate ("ManageLight_Timer" , 200, pdTRUE , NULL , appMENU_NormalManageLightISR);

	mqtt_timer = xTimerCreate ("MQTTPublish_Timer" , configTICK_RATE_HZ * mqtt_publish_interval_s, pdTRUE , NULL , appMENU_NormalPublishISR);

	char* menu_title = "                ";

	char* idle  = "                ";

	char* menu_names[] = {idle};
	void* args[] = {NULL};
	ItemHandler_t handlers[] = {appMENU_NormalAffixHandler};
	Menu_t* menu = uiMENU_Generate(menu_title, menu_names, handlers, args, 1);


	while(1){

	    xTimerStart(manage_light_timer, portMAX_DELAY);

	    netMQTT_Start();
    	xTimerStart(mqtt_timer, portMAX_DELAY);

    	uiMENU_Execute(menu);

    	xTimerStop(mqtt_timer, portMAX_DELAY);
    	netMQTT_Stop();

	    xTimerStop(manage_light_timer, portMAX_DELAY);

	    xTaskNotifyGive(maintenance_task);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	}

}


void appMENU_MaintenanceMode(void * arg){
	char* menu_title = "MAINTENANCE";

	char* clock_item = "set time";
	char* date_item = "set date";
	char* update_item = "sync time";
	char* minlight_item = "set light";
	char* wifi_item = "set wifi";
	char* mqtt_item = "set mqtt";

	char* menu_names[] = {clock_item, date_item, update_item, minlight_item, wifi_item, mqtt_item};
	void* args[] = {NULL, NULL, NULL, &light, NULL, NULL, NULL};
	ItemHandler_t handlers[] = {appMAINTENANCE_SetTimeHandler,
								appMAINTENANCE_SetDateHandler,
								appMAINTENANCE_SyncTimeHandler,
								appMAINTENANCE_SetLightHandler,
								appMAINTENANCE_SetWifiHandler,
								appMAINTENANCE_SetMQTTHandler};

	Menu_t* maint = uiMENU_Generate(menu_title, menu_names, handlers, args, 6);


	while(1){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		uiMENU_Execute(maint);
		xTaskNotifyGive(normal_task);

	}
}



int main(void) {

	RTOS_Init();
	NETWORK_Init();
	APPLICATION_Init();

	vTaskStartScheduler();

	while(1);

	return 0;
}











/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	/* This function will get called if a task overflows its stack. */

	( void ) pxTask;
	( void ) pcTaskName;

	for( ;; );
}
/*-----------------------------------------------------------*/



