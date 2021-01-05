#ifndef _PARAM_H_
#define _PARAM_H_
#include <Arduino.h>

#define DEV //development flag
/*********************************************************
 *                     Device Parameter
 ********************************************************/

/******************Device Identifier********************/
#define SENSOR_TYPE 2
#define COWSHED_ID 80

/******************Buffer Parameters*********************/
#define TOTAL_PAYLOAD_BUFFER 2

/*****************Gas Sensor Parameter******************/
#define MQ4_AIR_RS_R0_RATIO 1.0f
#define MQ135_AIR_RS_R0_RATIO 3.7f

/****************Interval Parameter********************/
#define DATA_ACQUIRE_INTERVAL  2


#endif
