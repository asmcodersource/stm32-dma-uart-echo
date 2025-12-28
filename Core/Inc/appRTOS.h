#ifndef __APP_RTOS_H
#define __APP_RTOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "queue.h"

extern osMessageQId uartRxQueueHandle;
extern osMessageQId uartTxQueueHandle;

#ifdef __cplusplus
}
#endif

#endif
