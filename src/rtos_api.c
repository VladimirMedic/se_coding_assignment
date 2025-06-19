#include "rtos_api.h"

/*This file contains mocks implementations for rtos functions*/
void rtos_task_delay_ms(uint32_t ms){};
void rtos_task_create(rtos_task_t* task, void (*task_fn)(void*), void* arg){};
void rtos_mutex_lock(void){};
void rtos_mutex_unlock(void){};
