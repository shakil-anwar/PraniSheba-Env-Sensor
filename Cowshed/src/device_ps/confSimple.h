#ifndef _CONFSIMPLE_H_
#define _CONFSIMPLE_H_
#include <Arduino.h>
#define BTN_DEFAULT false
#define SETUP_DONE_CONST  200

typedef struct config_t
{
  uint8_t isSetupDone;
  bool isDebugOff;
  uint16_t deviceId;
  uint16_t sampInterval;
};
typedef void (*memFun_t)(config_t*);

void confSetting(uint8_t pin,memFun_t read, memFun_t save);
extern config_t config;
#endif
