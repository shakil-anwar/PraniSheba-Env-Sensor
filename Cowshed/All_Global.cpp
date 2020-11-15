#include "All_Global.h"
#include "obj.h"
#include "time.h"
#include "dataSchema.h"
#include "radio.h"


uint8_t buff[256];

void updateDisplay();
//void sendPayload();


//Task task1(5, &sendPayload); //send payload triggers after 5 second interval
Task task2(2, &updateDisplay);
Scheduler scheduler;


void system_setup(void)
{
  Serial.begin(9600);
  pinMode(FLASH_CS, OUTPUT);
  pinMode(FLASH_CS, HIGH);
  radio_begin();
  objectsBegin();
  dataSchemaBegin();
//  rtcBegin();
  led_begin();

  humSensorBegin();
  mqBegin();
  mqCalibrate();
  radio_begin();
  
//  scheduler.addTask(&task1);
  scheduler.addTask(&task2);
  timeBegin();
//  toSetverBegin();
  
  scheduler.begin(&second);
  Serial.println("Setup Done");
  
  
}

void payloadStateMachine()
{
  memQ.saveLoop();
  server.sendLoop(1);
}
