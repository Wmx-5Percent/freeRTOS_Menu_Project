#ifndef __VARIABLES_H__
#define __VARIABLES_H__

#include "main.h"
#include "FreeRTOS.h"

typedef struct 
{
    TaskFunction_t pxTaskCode;
    const char* const pcName;
    uint16_t usStackDepth;
    void *const pvParameters;
    UBaseType_t uxPriority;
    TaskHandle_t *const pxCreatedTask;
}task_t;

#endif
