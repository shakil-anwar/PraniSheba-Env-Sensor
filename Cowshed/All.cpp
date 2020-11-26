#include "All.h"
#include "Schema.h"

#define DATA_ACQUIRE_INTERVAL   5
uint8_t buff[256];

void updateDisplay();
//void sendPayload();


Task task1(DATA_ACQUIRE_INTERVAL, &dataAcquisition); //send payload triggers after 5 second interval
Task task2(10, &updateDisplay);
Scheduler scheduler;


void system_setup(void)
{
  Serial.begin(250000);
  radio_begin();
  radioStart();
  
  pinMode(FLASH_CS, OUTPUT);
  pinMode(FLASH_CS, HIGH);
  
  led_begin();

  humSensorBegin();
  mqBegin();
  mqCalibrate();
  objectsBegin();
  dataSchemaBegin();

  scheduler.addTask(&task1);
  scheduler.addTask(&task2);
  scheduler.begin(&second);
  
  wdtEnable(8000);
  realTimeBegin(getRtcTime);
  delay(2000);
  
  Serial.println("Setup Done.");
}

bool isHardwareOk()
{
  return true;
}

void startDevice()
{
//  radioStart();
  wdtStart();
}

void deviceRunSM()
{
  memQ.saveLoop();
  server.sendLoop(1);
  realTimeSync();
}
