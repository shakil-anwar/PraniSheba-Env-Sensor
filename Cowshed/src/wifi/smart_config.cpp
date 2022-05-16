#include "smart_config.h"

enum smartConfigState_t softApState;

char softApSSID[30], softApPass[16];
unsigned int _soft_ap_timeout_minute;
boolean startApvalue;


/************************* Smart Config ***************************/

#define softApDebug 1   // set it to 1 for DEBUG_PRINT or set to 0 for no print

softApClass wifiManager;
//DNS config fist part="smiot" is set in wifimanager.cpp and second part="com" is set in ESP8266mDNS_Legacy.h
/************************************************************/


void setupSmartConfig(char *ap_name, unsigned int ap_timeout_minute)
{
  memset(softApSSID, '\0', 30);
  memset(softApPass, '\0', 16);
  serverMqtt.isConfigChanged = false;

  sprintf(softApSSID, "Pranisheba %s", ap_name);
  sprintf(softApPass, "%s%s", ap_name, ap_name);
  _soft_ap_timeout_minute = ap_timeout_minute;
  softApState = SMART_CONFIG_IDLE;
//  debugSerial.print(ap_name);
}

void handleSmartConfig()
{
  switch(softApState)
  {
    case SMART_CONFIG_START:
      softApState = SMART_CONFIG_WAIT;
      startApvalue = start_softap(softApSSID, softApPass, _soft_ap_timeout_minute);
      debugSerial.println("softApState: SMART_CONFIG_WAIT");
      debugSerial.print("softApvalue: ");debugSerial.println(startApvalue);
      break;

    case SMART_CONFIG_WAIT:
      if(!wifiManager.handleConfigPortal())
      {
        softApState = SMART_CONFIG_END;
      }
      break;

    case SMART_CONFIG_END:
      softApState = SMART_CONFIG_IDLE;
      break;
  }
}

boolean start_softap(char *ap_name, char *ap_pass, unsigned int ap_timeout_minute)
{
  int smartConfig = 0;
  wifiManager.setConfigPortalTimeout((ap_timeout_minute*60));
  wifiManager.startConfigPortal(ap_name, ap_pass);
  return true;
  
}
