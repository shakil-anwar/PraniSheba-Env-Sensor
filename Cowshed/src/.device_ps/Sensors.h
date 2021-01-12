#ifndef _SENSORS_H_
#define _SENSORS_H_
// #include "../IoT.h"
// #include "device.h"

/*****************Gas Sensor Parameter******************/
#define MQ4_AIR_RS_R0_RATIO 		1.0f
#define MQ135_AIR_RS_R0_RATIO 		3.7f

/******API for  Schema***************/
float getTemp();
float getHum();
float getAmmonia();
float getMethane();

/********API for final Integration***********/
bool sensorBegin();
bool sensorCalibrate();

#endif
