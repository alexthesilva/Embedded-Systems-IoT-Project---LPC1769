#include "rtostimeout.h"

TickType_t rtosTIMEOUT_Start(){
	 return xTaskGetTickCount();
}

bool rtosTIMEOUT_Expired(TickType_t start_time, TickType_t timeout){
	return xTaskGetTickCount() - start_time > timeout;
}

