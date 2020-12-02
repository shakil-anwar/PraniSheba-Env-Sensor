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
  //  realTimeBegin(getRtcTime);
  rtBegin();
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

void syncTime()
{
  uint8_t maxCount = 5;
  uint32_t uTime;
  do
  {
    uTime = getRtcTime();
    if (uTime)
    {
      Serial.print(F("Got NTP Time"));
      break;
    }
    else
    {
      Serial.print(F("Try count :")); Serial.println(maxCount);
      delay(1000);
    }
  } while (--maxCount);

  Serial.println(uTime);
  if(uTime)
  {
    Serial.println(F("Setting Time"));
    rtSync(uTime);
  }
  
}
