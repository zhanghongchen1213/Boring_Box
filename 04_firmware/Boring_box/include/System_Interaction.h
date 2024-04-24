#ifndef __SYSTEM_INTERACTION
#define __SYSTEM_INTERACTION

#include "System_Config.h"

#define IDLE_TIMEOUT_MS 60000 // 60s定时执行舔狗任务

#ifdef __cplusplus
extern "C"
{
#endif

    extern TaskHandle_t Servos_Open_Handle;
    extern TaskHandle_t Servos_Close_Handle;
    extern TaskHandle_t OLED_Handle;
    extern TaskHandle_t Mp3_Handle;
    extern TaskHandle_t Key_Handle;
    extern TaskHandle_t LickDog_Handle;

#ifdef __cplusplus
} // extern "C"
#endif

// C++ code
void Service_Init(void);

#endif