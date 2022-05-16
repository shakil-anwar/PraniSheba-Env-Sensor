#ifndef _CONFSIMPLE_H_
#define _CONFSIMPLE_H_
#include <Arduino.h>
#include  "../../IoT.h"

#if defined(PCB_V_0_1_0)
   #define BTN_DEFAULT true
#elif defined(PCB_V_0_2_0)
   #define BTN_DEFAULT false
#elif defined(ESP32_V010)
  #include <pgmspace.h>
  #define BTN_DEFAULT false
#else
  #error "Btn Board not selected"
#endif


#define SETUP_DONE_CONST  200

typedef struct config_t
{
  uint8_t isSetupDone;
  bool isDebugOff;
  uint16_t deviceId;
  uint16_t sampInterval;
}config_t;
typedef void (*memFcn_t)(config_t*);

void confSetting(uint8_t pin,memFcn_t read, memFcn_t save);
bool confIsOk(); //This function has to call after calling confSetting
extern config_t config;
#endif
