#include "ramQ.h"

#define TOTAL_BUFFER  2
typedef struct sensor_t
{
  int volt;
  int current;
};

sensor_t sensor[TOTAL_BUFFER];

void setup()
{
  Serial.begin(9600);
  ramQSet(&sensor, sizeof(sensor_t), TOTAL_BUFFER);
  Serial.println(3*sizeof(sensor_t));
  Serial.println(F("Setup done"));

}

void loop()
{
  
  if (Serial.available())
  {
    if (Serial.read() == 'd')
    {
      Serial.println(F("|----------------------------------------------|"));
      getData((sensor_t*)ramQHead());
      ramQUpdateHead();
    }
  }
  sensor_t *readPtr = (sensor_t*)ramQRead();
  if (readPtr != NULL)
  {
    
    Serial.println(F("<-----Read Data---->"));
    printSensor(readPtr);
    ramQUpdateTail();
  }
  if (_ramQFlash != NULL)
  {
    Serial.println(F("<======Flash Pointer Test========>"));
    sensor_t *sen = (sensor_t*)_ramQFlash;
    uint8_t fCount = 0;
    do
    {
      printSensor(sen);
      sen++;
      fCount++;
    } while (fCount < TOTAL_BUFFER / 2);
    _ramQFlash = NULL;
  }
}

sensor_t *getData(sensor_t *sensor)
{
  sensor -> volt = random(1, 10);
  sensor -> current = random(11, 20);
  Serial.println(F("Generated data"));
  Serial.print(F("Current :")); Serial.print(sensor -> volt);
  Serial.print(F(" | Volt :")); Serial.println(sensor -> current);

}

void printSensor(sensor_t *sensor)
{
  Serial.print(F("Current :")); Serial.print(sensor -> volt);
  Serial.print(F(" | Volt :")); Serial.println(sensor -> current);
}
