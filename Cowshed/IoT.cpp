#include "IoT.h"
#include "radio.h"

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
  Serial.println(F("Factory Resetting Device.."));
  //   memQ.reset();  //erase full flash mem
  memq -> reset(memq); 
//  nrfTxAddrReset(saveAddr); //reset tc addr
  nrfTxConfigReset(&nrfConfig, NRF_CONFIG_ROM_ADDR, eepromUpdate);
}

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
