#include "rtospir.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "pir.h"
#include <stdint.h>
#include "rtosrtc.h"
#include "timers.h"
#include <stdbool.h>
#include <stdio.h>

static bool presence = false;

static bool init = false;

static bool pir_init = false;

static TimerHandle_t pir_timer = NULL;

void rtosPIR_TMR(TimerHandle_t xTimer){
	if(!pir_init){
		PIR_Init();
		pir_init = true;
	}
	presence = PIR_GetValue();

}

bool rtosPIR_Init(){
	if(init)
		return false;
	TickType_t pir_period = pdMS_TO_TICKS(RTOS_PIR_PERIOD_MS);

	if((pir_timer =  xTimerCreate ("PIR_Timer" , pir_period, pdTRUE , NULL , rtosPIR_TMR)) == NULL)
		return false;

	PIR_Init();
	presence = PIR_GetValue();

    if (xTimerStart(pir_timer, portMAX_DELAY) != pdPASS)
    	return false;

    init = true;
	return true;
}


bool rtosPIR_GetValue(){
	return presence;
}

