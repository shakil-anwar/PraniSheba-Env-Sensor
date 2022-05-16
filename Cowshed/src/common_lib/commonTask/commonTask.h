#ifndef _COMMON_TASK_H_
#define _COMMON_TASK_H_
#include "Arduino.h"

void attachNtpFunc(uint32_t (*getNtpFunc)(void));
uint32_t getNtp(void);

void attchWifiConnect(bool (*wifiConnectFunc)(void));
bool isWifiAvailable();

void attchMqttFunc( bool (*mqttAvailFunc)(void));
bool isMqttOk();

uint16_t calcLen(uint8_t *buf);


void attachDataSendToServer( bool(*dataSendToServerFunc)(void *buffer, uint16_t len));
bool dataSendToServer(void *buffer, uint16_t len);

/******* to check QOS2 ack******/
void attachQos2AckFunc(bool (*mqttQosSendOkay)(uint8_t));
bool checkmqttQos2ack(uint8_t mid);

void espWdtBegin(uint8_t wdtPin);
void espWdtReset(bool bResetFlag = false);

#endif //_COMMON_TASK_H_