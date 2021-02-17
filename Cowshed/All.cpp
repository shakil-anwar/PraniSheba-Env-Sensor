#include "All.h"
#include "EEPROM.h"

//runState_t runState;
void system_setup(void)
{
  Serial.begin(250000);
  radio_begin();

  //  pinMode(FLASH_CS, OUTPUT);
  //  pinMode(FLASH_CS, HIGH);

  //  dataSchemaBegin();
  deviceBegin();
  objectsBegin();

  wdtEnable(8000);
  rtcBegin();
  rtAttachRTC(rtcGetSec, rtcUpdateSec);
  rtBegin();
  delay(1000);

  Serial.println("Setup Done.");
}

bool isHardwareOk()
{
  Serial.println(F("<--Hardware Status-->"));
  bool nrfOk = nrfIsRunning();
  Serial.print(F("NRF :")); Serial.println(nrfOk);
  Serial.print(F("RTC: ")); Serial.println(true);
  Serial.print(F("Flash :")); Serial.println(true);
  Serial.print(F("Logic Power: ")); Serial.println(true);
  Serial.println(F("<------------------->"));
  return true;
}

void startDevice()
{
  radioStart();
  wdtStart();
}

void deviceRunSM()
{
#if defined(DEVICE_HAS_FLASH_MEMORY)
  memQ.saveLoop();
#endif
  bool nrfsendok = xferSendLoop();
  if (nrfsendok == false)
  {
    Serial.print(F("==>send ok :")); Serial.println(nrfsendok);
    if(nrfPing()>0)
    {
      xferReady();
    }
  }
  static uint32_t prevModeMillis;
  if (millis() - prevModeMillis > 2000)
  {
    nrfWhichMode();
    prevModeMillis = millis();
  }
}


//bool setDeviceConf()
//{
//  memset(&queryBuffer, 0, sizeof(queryData_t));
//  nrfStandby1();
//  nrfTXStart();
//  confPacket_t *confPtr = (confPacket_t*)nrfQuery(0, 3);
//  nrfPowerDown();
//  if (confPtr != NULL)
//  {
//    Serial.print(F("\r\nType : ")); Serial.print(confPtr -> type);
//    Serial.print(F(" Opcode: ")); Serial.println(confPtr -> opCode);
//    Serial.print(F("Device addr :")); Serial.println(confPtr -> txAddrByte);
//    Serial.print(F("Time :")); Serial.println(confPtr -> uTime);
//    //    flash.printBytes((byte*)confPtr, sizeof(confPacket_t));
//    if (confPtr -> type == 0 && confPtr -> opCode == 3)
//    {
//      Serial.println(F("Setting Node as TX"));
//      nrfTxAddrRestore(confPtr -> txAddrByte);
//      rtSync(confPtr -> uTime);
//
//      nrfStandby1();
//      nrfTXStart();
//    }
//
//    return true;
//  }
//  else
//  {
//    Serial.println(F("Return Null"));
//    delay(2000);
//  }
//  return false;
//}
//
//bool syncTime()
//{
//  uint32_t uTime = getRtcTime();
//  if (uTime)
//  {
//    Serial.print(F("Got NTP Time : ")); Serial.println(uTime);
//    rtSync(uTime);
//    return true;
//  }
//  else
//  {
//    Serial.println(F("NTP Parse Failed"));
//    delay(1000);
//    return false;
//  }


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

//}


void saveAddr(addr_t *addrPtr)
{
  Serial.println(F("NRF EEPROM Saving.."));
  uint8_t *ptr = (uint8_t*)addrPtr;
  for (uint8_t i = 0; i < sizeof(addr_t); i++)
  {
    EEPROM.update(ROM_ADDR_FOR_TXD + i, *(ptr + i));
  }
  nrfDebugBuffer(ptr, sizeof(addr_t));
}

void readAddr(addr_t *addrPtr)
{
  Serial.println(F("NRF EEPROM Reading.."));
  uint8_t *ptr = (uint8_t*)addrPtr;
  for (uint8_t i = 0 ; i < sizeof(addr_t); i++)
  {
    *(ptr + i) = EEPROM.read(ROM_ADDR_FOR_TXD + i);
  }
  nrfDebugBuffer(ptr, sizeof(addr_t));
}
