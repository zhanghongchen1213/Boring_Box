#ifndef __SYSTEM_CONFIG_H
#define __SYSTEM_CONFIG_H

#include <Arduino.h>
#include <Wire.h>
#include "freertos/FreeRTOS.h"
#include "System_Interaction.h"

#include "oled.h"
#include "Mp3.h"
#include "key.h"
#include "SG90s.h"
#include "mode.h"
#include "debug.h"
#include "network.h"


/* Used for freertos */
#define Tiny_Stack_Size 64
#define Small_Stack_Size 128
#define Normal_Stack_Size 256
#define Large_Stack_Size 512
#define Huge_Stack_Size 1024
#define PriorityVeryLow 1
#define PriorityLow 2
#define PriorityBelowNormal 3
#define PriorityNormal 4
#define PriorityAboveNormal 5
#define PriorityHigh 6
#define PrioritySuperHigh 7
#define PriorityRealtime 8

void System_Resource_Init(void);
void System_Tasks_Init(void);
void POWERON_Task(void *pvParameters);

#endif