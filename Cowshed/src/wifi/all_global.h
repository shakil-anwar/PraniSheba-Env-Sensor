#ifndef _ALL_GLOBAL_H_
#define _ALL_GLOBAL_H_
#include <Arduino.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>

#if defined(PROD_BUILD)
#include "./lib/ESP8266Ping/src/ESP8266Ping.h"
#else
#include <ESP8266Ping.h>
#endif

#elif defined(ESP32_V010)
#include <esp_wifi.h>
#include <WiFi.h>

#if defined(PROD_BUILD)
#include "./lib/ESP32Ping/ESP32Ping.h"
#else
#include <ESP32Ping.h>
#endif


#endif
#include "wifi.h"
#include "mqtt.h"
#include "param.h"
#include "pin.h"
#include "utility.h"
#include "iotClient.h"
#include "subscription.h"
// #include "ota.h"

#if defined(PROD_BUILD)
#include "../common_lib/ringQ/ringQ.h"
#include "../common_lib/commonTask/commonTask.h"
#else
#include "ringQ.h"
#endif



void all_setup(void);
// #if defined(ESP8266)
// void pipeStateMachine(void);
// void handleOpcode10(void);
// void handleOpcode11(void);
// void handleOpcode12(void);
// void handleOpcode13(void);
// void handleOpcode20(void);
// void handleOpcode35();
// void handleOpcode14();
// #endif
void espDataSendLoop();
void espDataSendLoopMid();
#if defined(ESP32_V010)
bool checkQos2Ack(uint8_t mid);
bool espDataSendToServer(void *buffer, uint16_t len);
void mqttLoop();
extern volatile uint8_t midSendFlag;
#endif


//extern Pinger pinger;

#endif
