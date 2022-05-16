
#ifndef _SIM_800_L_
#define _SIM_800_L_

#define TINY_GSM_MODEM_SIM800
#include "mqtt.h"
#include <TinyGsmClient.h>
#include "./lib/ArduinoHttpClient/src/ArduinoHttpClient.h"


void simSetup();
void simBegin();
boolean simMqttConnect();
bool isGprsAvailable();
bool isGprsConnected();
uint32_t getGprsNtpTime();
uint32_t getGprsUnixTime();
bool gsmRadioOff();
bool gsmPowerOff();


extern TinyGsm modem;
extern PubSubClient  simMqtt;

#endif //_SIM_800_L_