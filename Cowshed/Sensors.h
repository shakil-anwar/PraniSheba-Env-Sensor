#ifndef _SENSORS_H_
#define _SENSORS_H_
#include "IoT.h"

/******API for  Schema***************/
float getTemp();
float getHum();
float getAmmonia();
float getMethane();

/********API for final Integration***********/
bool sensorBegin();
bool sensorCalibrate();

#endif
