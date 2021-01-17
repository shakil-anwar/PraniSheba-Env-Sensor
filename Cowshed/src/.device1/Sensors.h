#ifndef _SENSORS_H_
#define _SENSORS_H_

/******API for  Schema***************/
float getCurrent();
float getVoltage();
float getTemp();

/********API for final Integration***********/
bool sensorBegin();
bool sensorCalibrate();

#endif
