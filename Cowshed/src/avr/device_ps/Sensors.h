#ifndef _SENSORS_H_
#define _SENSORS_H_

#include "../lib/MQSensor/src/MQ.h"
// #include "device.h"
#include "./conf/confSimple.h"

// /*****************Gas Sensor Parameter******************/
// #define MQ4_AIR_RS_R0_RATIO 		4.4f
// #define MQ135_AIR_RS_R0_RATIO 		3.7f

/******API for  Schema***************/
float getTemp();
float getHum();
float getAmmonia();
float getMethane();

/********API for final Integration***********/
void sensorBegin();
bool sensorCalibrate();

extern MQ mq4;
extern MQ mq135;

#endif
