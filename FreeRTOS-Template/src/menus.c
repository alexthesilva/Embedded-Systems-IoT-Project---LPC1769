#include "menus.h"


#include "uimenu.h"
#include <FreeRTOS.h>
#include <netmqtt.h>
#include <semphr.h>
#include <stdbool.h>
#include "rtosled.h"
#include "rtoslcd.h"
#include "rtosrtc.h"
#include "rtosencoder.h"
#include "rtospir.h"
#include "rtosbh1750.h"
#include <stdio.h>
#include "rtosespserial.h"
#include "netntp.h"
#include "netmqtt.h"
#include "netwifi.h"
#include "string.h"
#include "timers.h"


char* wday[7]={"SUN","MON","TUE","WED","THU","FRI","SAT"};

char chars[] = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ 0123456789";

char nums[] = "0123456789";


char* appMENU_GetWeekDay(int index){
	return wday[index];
}

//-----------------SETTIME-----------------------------------

bool appMAINTENANCE_xSetTimeHandler(void* arg){
	bool ret;
	int res = 0;
	struct tm* dateTime = pvPortMalloc(sizeof(struct tm));
	rtosRTC_GetValue(dateTime);
	char* input = pvPortMalloc(16);
	char* output = pvPortMalloc(16);
	sprintf (input, "%02d:%02d:%02d",dateTime->tm_hour,dateTime->tm_min,dateTime->tm_sec);

	uiMENU_InputData("hh:mm:ss", input, strlen(input), nums, output);

	res = sscanf(output, "%02d:%02d:%02d", &(dateTime->tm_hour), &(dateTime->tm_min), &(dateTime->tm_sec));
	if(res == 3){
		rtosRTC_SetValue(dateTime);
		ret = true;
	}
	else
		ret = false;

	vPortFree(dateTime);
	vPortFree(input);
	vPortFree(output);

	return ret;
}

void appMAINTENANCE_SetTimeHandler(void* arg){
	uiMENU_ExecuteCMD("SET TIME", appMAINTENANCE_xSetTimeHandler, arg);
}


//-----------------SETDATE-----------------------------------

bool appMAINTENANCE_xSetDateHandler(void* arg){
	bool ret;
	int res = 0;
	struct tm* dateTime = pvPortMalloc(sizeof(struct tm));
	rtosRTC_GetValue(dateTime);
	char* input = pvPortMalloc(16);
	char* output = pvPortMalloc(16);
	sprintf (input, "%04d/%02d/%02d", dateTime->tm_year, dateTime->tm_mon, dateTime->tm_mday);

	uiMENU_InputData("yyyy/MM/dd", input, strlen(input), nums, output);

	res = sscanf(output, "%04d/%02d/%02d", &(dateTime->tm_year), &(dateTime->tm_mon), &(dateTime->tm_mday));
	if(res == 3){
		dateTime->tm_wday = uiMENU_SelectOptions("WEEKDAY", wday, 7, false);
		rtosRTC_SetValue(dateTime);
		ret = true;
	}
	else
		ret = false;

	vPortFree(input);
	vPortFree(output);
	vPortFree(dateTime);

	return ret;
}

void appMAINTENANCE_SetDateHandler(void* arg){
	uiMENU_ExecuteCMD("SET TIME", appMAINTENANCE_xSetDateHandler, arg);
}


//-----------------SYNCTIME-----------------------------------

bool appMAINTENANCE_xSyncTime(void* arg){
	netMQTT_Stop();
	struct tm date;
	time_t secs_since_1900 = netNTP_GetTime(DEFAULT_UTC_ADJUST);
	if(secs_since_1900){
		date = *localtime(&secs_since_1900);
		date.tm_mon += 1;
		date.tm_year += 1900;
		rtosRTC_SetValue(&date);;
		return true;
	}
	return false;
}


void appMAINTENANCE_SyncTimeHandler(void* arg){
	uiMENU_ExecuteCMD("SYNC TIME", appMAINTENANCE_xSyncTime, arg);
}

//-----------------SETLIGHT-----------------------------------

bool appMAINTENANCE_xSetMinLightHandler(void* arg){
	int* light = (int*) arg;
	char* input = pvPortMalloc(7);
	char* output = pvPortMalloc(7);
	sprintf (input, "%06d", *light);
	bool ret;
	uiMENU_InputData("MIN. LIGHT", input, strlen(input), nums, output);
	int res = sscanf(output, "%06d", light);
	if(res == 1){
		ret = true;
	}
	else
		ret = false;
	vPortFree(input);
	vPortFree(output);
	return ret;
}

void appMAINTENANCE_SetMinLightHandler(void* arg){
	uiMENU_ExecuteCMD("MIN. LIGHT", appMAINTENANCE_xSetMinLightHandler, arg);
}

bool appMAINTENANCE_xLightModeHandler(void* arg){
	char* mode_names[] = {"unconfigured",
			   "cont. hi-res",
			   "cont. hi-res2",
			   "cont. low-res",
			   "1time hi-res",
			   "1time hi-res2",
			   "1time low-res"};
	BH1750_ModeType modes[] = {UNCONFIGURED,
							   CONTINUOUS_HIGH_RES_MODE,
							   CONTINUOUS_HIGH_RES_MODE_2,
							   CONTINUOUS_LOW_RES_MODE,
							   ONE_TIME_HIGH_RES_MODE,
							   ONE_TIME_HIGH_RES_MODE_2,
							   ONE_TIME_LOW_RES_MODE};
	int index = uiMENU_SelectOptions("LIGHT_MODE", mode_names, 7, true);
	return (index < 0) ? false : rtosBH1750_ConfigureMode(modes[index]);

}

void appMAINTENANCE_LightModeHandler(void* arg){
	uiMENU_ExecuteCMD("LIGHT MODE", appMAINTENANCE_xLightModeHandler, arg);
}

bool appMAINTENANCE_xLightMeasHandler(void* arg){

	char* mode_names[] = {"default", "min meas.", "max meas."};
	BH1750_MeasurementTimeType modes[] = {DEFAULT_MEASUREMENT_TIME,
										  MIN_MEASUREMENT_TIME,
										  MAX_MEASUREMENT_TIME
										  };
	int index = uiMENU_SelectOptions("MEAS. TIME", mode_names, 3, true);
	return (index < 0) ? false : rtosBH1750_SetMeasurementTime(modes[index]);

}

void appMAINTENANCE_LightMeasHandler(void* arg){
	uiMENU_ExecuteCMD("MEAS. TIME", appMAINTENANCE_xLightMeasHandler, arg);
}




void appMAINTENANCE_SetLightHandler(void* arg){
	int* light = (int*) arg;
	char* menu_title = "SET LIGHT";

	char* minlight_item = "min. light";
	char* mode_item = "light mode";
	char* meas_item = "meas. time";

	char* menu_names[] = {minlight_item, mode_item, meas_item};
	void* args[] = {light, NULL, NULL};
	ItemHandler_t handlers[] = {appMAINTENANCE_SetMinLightHandler, appMAINTENANCE_LightModeHandler, appMAINTENANCE_LightMeasHandler};
	Menu_t* light_menu = uiMENU_Generate(menu_title, menu_names, handlers, args, 3);
	uiMENU_Execute(light_menu);

	uiMENU_Destroy(light_menu);
}


//-----------------SETWIFI-----------------------------------


bool appMAINTENANCE_xScanNetworks(void* arg){
	bool ret;
	char* scanned_ssids[MAX_AP_SCAN];
	int scanned;
	netWIFI_Scan(MAX_AP_SCAN, &scanned);
	char * password_out = pvPortMalloc(65);
	bool makedefault;
	if(scanned > 0){
		for(int i = 0; i < scanned; i++){
			scanned_ssids[i] = (netWIFI_ScannedSSID(i))->ssid;
		}
		int res = uiMENU_SelectOptions("SELECT NETWORK", scanned_ssids, scanned, true);
		if(res >= 0){
			uiMENU_InputData("ENTER PASSWORD", NULL, 64, chars, password_out);
			makedefault = uiMENU_Ask("DEFAULT?");

			ret = netWIFI_Connect(scanned_ssids[res], password_out, makedefault);
		}
		else
			ret = false;
	}
	else
		ret = false;
	vPortFree(password_out);
	return ret;
}

void appMAINTENANCE_ScanNetworks(void* arg){
	uiMENU_ExecuteCMD("SCAN NETWORKS", appMAINTENANCE_xScanNetworks, arg);
}




bool appMAINTENANCE_xJoinNetwork(void* arg){
	bool ret;

	char* ssid_out = pvPortMalloc(65);
	char* password_out = pvPortMalloc(65);
	bool makedefault;

	uiMENU_InputData("ENTER SSID", NULL, 64, chars, ssid_out);

	uiMENU_InputData("ENTER PASSWORD", NULL, 64, chars, password_out);

	makedefault = uiMENU_Ask("DEFAULT?");

	ret = netWIFI_Connect(ssid_out, password_out, makedefault);

	vPortFree(password_out);
	vPortFree(ssid_out);
	return ret;
}

void appMAINTENANCE_JoinNetwork(void* arg){
	uiMENU_ExecuteCMD("JOIN NETWORK", appMAINTENANCE_xJoinNetwork, arg);
}


bool appMAINTENANCE_xDisconnectNetwork(void* arg){
	bool ret;

	ret = netWIFI_Disconnect();

	return ret;
}


void appMAINTENANCE_DisconnectNetwork(void* arg){
	uiMENU_ExecuteCMD("DISCONNECTING...", appMAINTENANCE_xDisconnectNetwork, arg);
}


void appMAINTENANCE_SetWifiHandler(void* arg){
	char* menu_title = "SET WIFI";

	char* scan_item = "scan networks";
	char* join_item = "join network";
	char* disconnect_item = "disconnect";

	char* menu_names[] = {scan_item, join_item, disconnect_item};
	void* args[] = {NULL, NULL, NULL};
	ItemHandler_t handlers[] = {appMAINTENANCE_ScanNetworks,
								appMAINTENANCE_JoinNetwork,
								appMAINTENANCE_DisconnectNetwork};
	Menu_t* wifi_menu = uiMENU_Generate(menu_title, menu_names, handlers, args, 3);
	uiMENU_Execute(wifi_menu);

	uiMENU_Destroy(wifi_menu);
}

//-----------------SETMQTT-----------------------------------

bool appMAINTENANCE_xSetCredentials(void* arg){
	bool ret;

	char* address = pvPortMalloc(33);
	char* token = pvPortMalloc(33);
	char* port_str = pvPortMalloc(6);
	char* keepalive_str  = pvPortMalloc(6);
	int port;
	int keepalive;

	int res1;
	int res2;

	uiMENU_InputData("ENTER ADDRESS", NULL, 33, chars, address);

	uiMENU_InputData("ENTER TOKEN", NULL, 33, chars, token);

	uiMENU_InputData("ENTER PORT", NULL, 6, nums, port_str);

	uiMENU_InputData("ENTER KPALIVE[S]", NULL, 6, nums, keepalive_str);

	res1 = sscanf(port_str, "%d", &port);

	res2 = sscanf(port_str, "%d", &keepalive);

	if((res1 == 1) && (res2 == 1)){
		netMQTT_Config(address, token, (unsigned long) port, (unsigned long) keepalive);
		ret = true;
	}
	else
		ret = false;

	vPortFree(address);
	vPortFree(token);
	vPortFree(port_str);
	vPortFree(keepalive_str);

	return ret;
}

void appMAINTENANCE_SetMQTTHandler(void* arg){
	uiMENU_ExecuteCMD("SET MQTT", appMAINTENANCE_xSetCredentials, arg);
}

/*
void appMAINTENANCE_SetMQTTHandler(void* arg){
	TimerHandle_t mqtt_timer = (TimerHandle_t) arg;

	char* menu_title = "SET MQTT";

	char* set_item = "set credentials";
	char* stop_item = "stop mqtt";
	char* start_item = "start mqtt";
	char* delay_item = "set pub. delay";


	char* menu_names[] = {set_item, stop_item, start_item, delay_item};
	void* args[] = {NULL, mqtt_timer, mqtt_timer, mqtt_timer};
	ItemHandler_t handlers[] = {appMAINTENANCE_SetCredentials,
								appMAINTENANCE_MQTTStop,
								appMAINTENANCE_MQTTStart,
								appMAINTENANCE_MQTTDelay};
	Menu_t* mqtt_menu = uiMENU_Generate(menu_title, menu_names, handlers, args, 4);

	uiMENU_Execute(mqtt_menu);

	uiMENU_Destroy(mqtt_menu);

}*/
