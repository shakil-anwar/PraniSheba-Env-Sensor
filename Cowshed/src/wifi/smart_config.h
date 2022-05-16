#ifndef _ESP_SMART_CONFIG_
#define _ESP_SMART_CONFIG_

#if defined(PROD_BUILD)
#include "./lib/WiFiManager/softApClass.h"
#include "./lib/calibration/calibration.h"
#else
#include <softApClass.h>
#endif


enum smartConfigState_t
{
  SMART_CONFIG_START,
  SMART_CONFIG_WAIT,
  SMART_CONFIG_END,
  SMART_CONFIG_STOP,
  SMART_CONFIG_IDLE,
};

boolean start_softap(char *ap_name, char *ap_pass, unsigned int ap_timeout_minute);
void setupSmartConfig(char *ap_name, unsigned int ap_timeout_minute);
void handleSmartConfig();

extern enum smartConfigState_t softApState;


#endif
