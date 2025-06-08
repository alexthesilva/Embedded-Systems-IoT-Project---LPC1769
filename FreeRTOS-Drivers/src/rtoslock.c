#include "rtoslock.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct LockStruct {
	TaskHandle_t task;
	SemaphoreHandle_t mutex;
} Lock_t;

LockHandle_t rtosLOCK_Init(){

	Lock_t * ret = pvPortMalloc(sizeof(Lock_t));

	if((ret->mutex = xSemaphoreCreateMutex()) == NULL)
		return NULL;

	return ret;

}

void rtosLOCK_Begin(LockState_t lock, LockHandle_t handle){
	if(lock != LOCK_NONE)
		if(xTaskGetCurrentTaskHandle() !=  handle->task)
			if(xSemaphoreTake(handle->mutex, portMAX_DELAY) == pdPASS)
				if(lock == LOCK_BEGIN || lock == LOCK_SINGLE)
					handle->task = xTaskGetCurrentTaskHandle();
}

void rtosLOCK_End(LockState_t lock, LockHandle_t handle){
	if((lock != LOCK_NONE) && ((lock == LOCK_END) || (lock == LOCK_SINGLE)))
		if(xTaskGetCurrentTaskHandle() == handle->task){
			handle->task = NULL;
			xSemaphoreGive(handle->mutex);
			vTaskDelay(1);
		}
}

void rtosLOCK_Destroy(LockHandle_t lock){
	vSemaphoreDelete(lock->mutex);
	vPortFree(lock);
}
