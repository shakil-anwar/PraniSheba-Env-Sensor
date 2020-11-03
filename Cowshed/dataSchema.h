#ifndef _DATA_SCHEMA_H_
#define _DATA_SCHEMA_H_
#include <Arduino.h>

/*********************Global Dataschema Parameters***************/
#define FLASH_MEM_PAGE_BYTES        256
#define MAX_PAYLOAD_BYTES           32     //Payload  bytes should be even number        
#define MAX_PAYLOADS_IN_FLASH_PAGE  (FLASH_MEM_PAGE_BYTES/MAX_PAYLOAD_BYTES) //256/32 = 8
#define TOTAL_FLASH_BUFFERS          2 //This must be even numbers for equal division. one half for flash storing, other half for nrf data parsing
#define TOTAL_PAYLOAD_BUFFERS        (MAX_PAYLOADS_IN_FLASH_PAGE*TOTAL_FLASH_BUFFERS)

/********************Bolus Parameters****************************/
#define BOLUS_SAMPLE_IN_PACKET      8    //Total Samples in a packet
#define BOLUS_SAMPLE_INTERVAL       1000 //interval in milisecond
#define BOLUS_ID_PREFIX             'S'
/*********************Gas Sensor Parameters**********************/
#define SENSOR_TYPE 2
#define COWSHED_ID 100

typedef struct bolus_t
{
  uint8_t type;
  uint8_t reserve;

  uint8_t x[BOLUS_SAMPLE_IN_PACKET];
  uint8_t y[BOLUS_SAMPLE_IN_PACKET];
  uint8_t z[BOLUS_SAMPLE_IN_PACKET];

  uint16_t id;
  uint16_t temp;
  uint16_t utime;
};

typedef struct sensor_t
{
  uint8_t type;
  uint16_t id;
  uint32_t unixTime;
  float ammonia;
  float methane;
  float hum;
  float temp;
};

typedef union payload_t
{
  bolus_t bolus;
  sensor_t gasSensor;
};

typedef struct buffer_t 
{
  payload_t  payload[TOTAL_PAYLOAD_BUFFERS];
  payload_t *nrfPtr;
  payload_t *flashPtr;
  uint8_t pIndex; //pointer index.
  uint8_t tIndex; //total index
};


void dataSchemaBegin();
sensor_t *getSensorsData();
void printSensor(sensor_t *sensor);

void mqBegin();
void mqCalibrate();
void humSensorBegin();

float getTemp();
float getHum();
float getAmmonia();
float getMethane();
void updateDisplay();

extern volatile buffer_t buffer;

#endif 
