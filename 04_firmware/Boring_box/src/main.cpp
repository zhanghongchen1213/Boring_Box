#include "System_Config.h"

void setup()
{
  System_Resource_Init();
}

void loop()
{
  mqtt.loop();
}