#include "dataSchema.h"
#include "led.h"
#include "pin.h"

sensor_t sensor; //global variable for sensor data handling.

/**********Global variable for Payload Buffering ************/
volatile buffer_t buffer;

void dataSchemaBegin()
{
  //  Serial.println(sizeof(bolus_t));
  /**************Compile time checking************************/
  static_assert(!(TOTAL_FLASH_BUFFERS % 2), "\"TOTAL_FLASH_BUFFERS\" Should be Even Number");
  static_assert(!(MAX_PAYLOAD_BYTES % 2), "\"MAX_PAYLOAD_BYTES\" Should be Even Number");
  static_assert (sizeof(payload_t) == MAX_PAYLOAD_BYTES, "payload_t size is not equal definden size");
  /**************Done checking***********************************/
  buffer.pIndex   = 0;
  buffer.tIndex   = 0;
  buffer.nrfPtr   = (payload_t*)&buffer.payload[0];
  buffer.flashPtr = NULL; //Initially null
  
  //Initialise sensor constant parameter
  sensor.type  = SENSOR_TYPE;
  sensor.id = COWSHED_ID;
}

sensor_t *getSensorsData()
{
  sensor.temp = getTemp();
  sensor.hum = getHum();
  sensor.ammonia = getAmmonia();
  sensor.methane = getMethane();
  return &sensor;
}


void printSensor(sensor_t *sensor)
{
  Serial.println(F("<---------Sensor Data----------------->"));
  Serial.print(F("ID : "));Serial.println(sensor -> id );
  Serial.print(F("Temperature : "));Serial.print(sensor -> temp );Serial.println(" C");
  Serial.print(F("Humidity : "));Serial.print(sensor -> hum );Serial.println(" %Rh");
  Serial.print(F("Ammonia : "));Serial.print(sensor -> ammonia );Serial.println(" ppm");
  Serial.print(F("Methane : "));Serial.print(sensor -> methane );Serial.println(" ppm");
}

void updateDisplay()
{
  sensor_t *sensorPtr = getSensorsData();
  printSensor(sensorPtr);
  uint8_t level;
  //humidity
  level = (uint8_t)sensorPtr -> hum;
  level = level/20;
  led_set_level(1, level) ;
  
  //Temperature
   level = (uint8_t)sensorPtr -> temp;
  level = level/10;
  led_set_level(2, level);

  //Methane
  level = (uint8_t)sensorPtr -> methane;
  level = level/10;
  led_set_level(3, level);

  //Ammonia
  level = (uint8_t)sensorPtr -> ammonia;
  level = level/10;
  led_set_level(4, level);
}
