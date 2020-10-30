#include "All_Global.h"
#include "tScheduler.h"
#include "time.h"
#include "dataSchema.h"
#include "radio.h"


void sendPayload();

Task task1(5, &sendPayload); //send payload triggers after 5 second interval
Scheduler scheduler;


void system_setup(void)
{
  Serial.begin(9600);
  rtcBegin();
  led_begin();

  humSensorBegin();
  mqBegin();
  mqCalibrate();
  radio_begin();
  
  scheduler.addTask(&task1);
  scheduler.begin(second);
  timeBegin();
  Serial.println("Setup Done");
}

void sendPayload()
{
  Serial.println(F("Payload sending.."));
  sensor_t *sensorPtr = getSensorsData();
  printSensor(sensorPtr);
  //handle nrf data sending here 
  
}
