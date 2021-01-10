#ifndef _PAYLOAD_H_
#define _PAYLOAD_H_

#include "device.h"
#include "Schema.h"


void dataSchemaBegin();
sensor_t *getSensorsData(sensor_t *senPtr);
void printSensor(sensor_t *sensor);

void dataAcquisition();

extern volatile payload_t  payload[TOTAL_PAYLOAD_BUFFER];
extern queryData_t queryBuffer;
extern sensor_t sensor;

#endif 
