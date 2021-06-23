#include "IoT.h"
#include "radio.h"
#include "All.h"


void updateLog();

struct gasSensorLog_t sensorLog;

//RTC_DS1307 rtc;
Scheduler scheduler;
void objectsBegin()
{
  scheduler.begin(&second);
  xferBegin(deviceMemRead, deviceRfSend, deviceRfAckWait, millis);
  xferReady();
}

void factoryReset()
{
  Serial.println(F("Factory Resetting.."));
  //   memQ.reset();  //erase full flash mem
  memqReset(&memq); 
//  nrfTxAddrReset(saveAddr); //reset tc addr
  nrfTxConfigReset(&nrfConfig, NRF_CONFIG_ROM_ADDR, eepromUpdate);
#if defined(DEVICE_HAS_LOG)
  resetLog();
#endif
}

void gpioBegin()
{
  pinMode(FLASH_CS,OUTPUT);
  digitalWrite(FLASH_CS,HIGH);
}

#if defined(DEVICE_HAS_LOG)

void initiateLog()
{

  eepromRead(LOG_SAVE_ADDR, (uint8_t *)&sensorLog, sizeof(struct gasSensorLog_t));

  uint8_t logChecksum = sensorLog.header.checksum;
  sensorLog.header.checksum = 0;
  if((sensorLog.header.type != SENSOR_LOG_TYPE)
      || (sensorLog.header.id != config.deviceId)
      || (checksum((void *)&sensorLog,sizeof(struct gasSensorLog_t)) != logChecksum))
  {
    Serial.println("log>reset.");
    resetLog();
  }
  sensorLog.slotMissed = 1;
  sensorLog.restartCount++;
  updateLog();
  
}

void resetLog(void)
{
  memset((uint8_t *)&sensorLog, '\0', sizeof(struct gasSensorLog_t));
  sensorLog.header.type = SENSOR_LOG_TYPE;
  sensorLog.header.id = config.deviceId;
  sensorLog.restartCount = 0;
  updateLog();
  
}
void updateLog()
{
  sensorLog.header.checksum = 0;
  sensorLog.header.checksum  = checksum((void *)&sensorLog,sizeof(struct gasSensorLog_t)); 
  eepromUpdate(LOG_SAVE_ADDR, (uint8_t *)&sensorLog, sizeof(struct gasSensorLog_t));
}
 
struct gasSensorLog_t *saveLog()
{
  struct gasSensorLog_t *senLogPtr = (struct gasSensorLog_t*)ramQHead();

  sensorLog.header.type = SENSOR_LOG_TYPE;
  sensorLog.header.id = config.deviceId;
  
  sensorLog.errorCode = (tdmSyncState<<2) | (memq.ringPtr._isLock<<1) | ramqIsLocked() ;
  sensorLog.hardwareErrorCode = (nrfIsRunning()<<1) | (rtcIsRunning()<<2);
  sensorLog.railVoltage = getRailVoltage();
  sensorLog.unixTime = second();
  sensorLog.flashAvailablePackets = memqAvailable(&memq);
  sensorLog.samplingFreq = config.sampInterval;
  sensorLog.header.checksum = 0;
  sensorLog.header.checksum  = checksum((void *)&sensorLog,sizeof(struct gasSensorLog_t)); 
  
  if(senLogPtr != NULL)
  {
    memset(senLogPtr, '\0',32);
    memcpy(senLogPtr, &sensorLog, sizeof(struct gasSensorLog_t));
    ramQUpdateHead();
    memqSave();
  }

  
  return senLogPtr;
}

#endif
  

// void rtcBegin()
// {
//   if (! rtc.begin())
//   {
//     Serial.println(F("RTC Not Found"));
//   }
//   else
//   {
//     Serial.println(F("RTC Found"));
//   }
//   if (!rtc.isrunning())
//   {
//     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//     Serial.println(F("RTC Adjusted"));
//   }
//   else
//   {
//     Serial.println(F("-->RTC is Running"));
//   }
// }

// uint32_t rtcGetSec()
// {
//   uint32_t utime = 0;
//   if (rtc.isrunning())
//   {
//     Serial.println(F("Getting RTC time"));
//     DateTime now = rtc.now();
//     utime =  now.unixtime();
//   }
//   return utime;
// }
// void rtcUpdateSec(uint32_t unix)
// {
//   if (rtc.isrunning())
//   {
//     uint32_t rtcSec = rtcGetSec();
//     Serial.print(F("RTC Sec: ")); Serial.println(rtcSec);
//     if (abs((int32_t)(unix - rtcSec)) > 1)
//     {
//       //if difference is more than 1 sec, update rtc
//       Serial.println(F("Updating RTC Time"));
//       rtc.adjust(DateTime(unix));
//     }
//     else
//     {
//       Serial.println(F("RTC time is updated"));
//     }
//   }
// }
