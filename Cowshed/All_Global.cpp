#include "All_Global.h"
#include "time.h"
#include "dataSchema.h"
#include "radio.h"


uint8_t buff[256];

void updateDisplay();
//void sendPayload();


//Task task1(5, &sendPayload); //send payload triggers after 5 second interval
Task task2(5, &updateDisplay);
Scheduler scheduler;


void system_setup(void)
{
  Serial.begin(9600);
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
  toSetverBegin();
  
  scheduler.begin(&second);
  Serial.println("Setup Done");
  pinMode(FLASH_CS, OUTPUT);
//  FLASH_DIS();
  
}

void payloadStateMachine()
{
  memQ.saveLoop();
  dataSendStateMachine();
}
