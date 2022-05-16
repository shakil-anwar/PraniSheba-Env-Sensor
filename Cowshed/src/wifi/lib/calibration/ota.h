#ifndef _OTA_H_
#define _OTA_H_

#if defined(ESP32_V010)
// #include "iotClient.h"
// #include "param.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
// #include <WiFiClientSecure.h>


void otaBegin();
void otaLoop();
void firmwareUpdate(void);
int FirmwareVersionCheck(void);


// #include <ESPmDNS.h>
// #include <WiFiUdp.h>
// #include <ArduinoOTA.h>

// void otaBegin();
// void otaLoop();
#endif //ESP32_V010

#endif // endOTA