#ifndef _ALL_GLOBAL_H
#define _ALL_GLOBAL_H
#include "Arduino.h"
#include "MQ.h"
#include "led.h"
//#include "DHT.h"
#include "radio.h"

void system_setup(void);

typedef struct packet_t 
{
  char sid[6];
  float ammonia;
  float methane;
  float hum;
  float temp;
};

bool setSensorId();
packet_t *getSenorReading();

bool sendDataToBs();
extern MQ mq4;
//extern DHT dht;  
#endif 
