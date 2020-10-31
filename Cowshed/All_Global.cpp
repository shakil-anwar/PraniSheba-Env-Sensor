#include "All_Global.h"
#include "time.h"
#include "dataSchema.h"
#include "radio.h"

void updateDisplay();
void sendPayload();

Task task1(5, &sendPayload); //send payload triggers after 5 second interval
Task task2(2, &updateDisplay);
Scheduler scheduler;


void system_setup(void)
{
  Serial.begin(9600);
  dataSchemaBegin();
//  rtcBegin();
  led_begin();

//  humSensorBegin();
  mqBegin();
  mqCalibrate();
  radio_begin();
  
  scheduler.addTask(&task1);
  scheduler.addTask(&task2);
  timeBegin();
  scheduler.begin(&second);
  Serial.println("Setup Done");
}

void sendPayload()
{
  Serial.print(F("Payload sending:  "));Serial.println(second());
  sensor_t *sensorPtr = getSensorsData();
//  printSensor(sensorPtr);
  //handle nrf data sending here 
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
