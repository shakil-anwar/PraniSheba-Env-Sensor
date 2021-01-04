#include "All.h"
#include "Schema.h"


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
  //  radioStart();

  pinMode(FLASH_CS, OUTPUT);
  pinMode(FLASH_CS, HIGH);

  led_begin();

  //  humSensorBegin();
  //  mqBegin();
  //  mqCalibrate();
  sensorBegin();
  sensorCalibrate();

  objectsBegin();
  dataSchemaBegin();

  scheduler.addTask(&task1);
  scheduler.addTask(&task2);
  scheduler.begin(&second);

  wdtEnable(8000);
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
  radioStart();
  wdtStart();
}

void deviceRunSM()
{
  //  nrfDebugPrint();
  memQ.saveLoop();
//  server.sendLoop(1);
  bool nrfsendok = xferSendLoop();
  if(nrfsendok == false)
  {
    Serial.print(F("==>send ok :"));Serial.println(nrfsendok);
    xferReady();
  }
  realTimeSync();
  
  static uint32_t prevModeMillis;
  if (millis() - prevModeMillis > 2000)
  {
    nrfWhichMode();
    prevModeMillis = millis();
  }
}


bool setDeviceConf()
{
  memset(&queryBuffer, 0, sizeof(queryData_t));
  nrfStandby1();
  nrfTXStart();
  confPacket_t *confPtr = (confPacket_t*)nrfQuery(0, 3);
  nrfPowerDown();
  if (confPtr != NULL)
  {
    Serial.print(F("\r\nType : ")); Serial.print(confPtr -> type);
    Serial.print(F(" Opcode: ")); Serial.println(confPtr -> opCode);
    Serial.print(F("Device addr :")); Serial.println(confPtr -> txAddrByte);
    Serial.print(F("Time :")); Serial.println(confPtr -> uTime);
    //    flash.printBytes((byte*)confPtr, sizeof(confPacket_t));
    if (confPtr -> type == 0 && confPtr -> opCode == 3)
    {
      Serial.println(F("Setting Node as TX"));
      nrfTxAddrRestore(confPtr -> txAddrByte);
      rtSync(confPtr -> uTime);

      nrfStandby1();
      nrfTXStart();
    }

    return true;
  }
  else
  {
    Serial.println(F("Return Null"));
    delay(2000);
  }
  return false;
}

bool syncTime()
{
  uint32_t uTime = getRtcTime();
  if (uTime)
  {
    Serial.print(F("Got NTP Time : ")); Serial.println(uTime);
    rtSync(uTime);
    return true;
  }
  else
  {
    Serial.println(F("NTP Parse Failed"));
    delay(1000);
    return false;
  }


  //  do
  //  {
  //    uTime = getRtcTime();
  //    if (uTime)
  //    {
  //      Serial.print(F("Got NTP Time"));
  //      break;
  //    }
  //    else
  //    {
  //      Serial.print(F("Try count :")); Serial.println(maxCount);
  //      delay(1000);
  //    }
  //  } while (--maxCount);
  //
  //  Serial.println(uTime);
  //  if(uTime)
  //  {
  //    Serial.println(F("Setting Time"));
  //    rtSync(uTime);
  //  }

}
