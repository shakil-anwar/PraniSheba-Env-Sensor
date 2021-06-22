#include "Timers.h"
#include "IoT.h"
#include "radio.h"
//#include "RTClib.h"
//#include "AVR_Timer1.h"

void timer1Start();
void updateSec(uint32_t sec);
void timerIsr(void);




uint32_t getNtpTime();

volatile uint32_t _second;
volatile uint32_t _tempSec;
volatile uint32_t _nextSlotSec;
volatile bool _readyToSend;

// RTC_DS1307 rtc;

void TimersBegin()
{
   _readyToSend = false;
  _second = 0;
  _tempSec = 0;
  timer1.initialize(1);
  timer1.attachIntCompB(timerIsr);

  // rtcBegin();
  // rtAttachRTC(rtcGetSec, rtcUpdateSec);
  rtAttachFastRTC(second, updateSec, timer1Start);
  rtBegin();
}

void timerIsr(void)
{
  _second++;
  _tempSec++;
  Serial.println(_second);
  // Serial.println(F("Timer ISR Triggered"));
//  if(_second >=_slotSec)
//  {
//    if( _second < (_slotSec + nrfConfig.perNodeInterval))
//    {
//      _readyToSend = true;
//    }
//    else
//    {
//      _readyToSend = false;
//    }
//  }
}

//void setNextSlotSec(uint32_t unix)
//{
//  Serial.println(F("=====>Next Slot Second updated"));
//  _slotSec = unix;
//}
//
//bool *isTimeTosend()
//{
//  return &_readyToSend;
//}

void timer1Start()
{
  timer1.start();
}
uint32_t ms()
{
  // 1000*TCNT1/ICR1 + second*1000;
  uint16_t tcnt1 = TCNT1;
  // Serial.println(F("------"));
  // Serial.println(tcnt1);
  // Serial.println(ICR1);
  uint32_t temp = (uint32_t)tcnt1 * (uint32_t)1000;
  // Serial.println(temp);
  temp = temp / (uint32_t)ICR1;
  // Serial.println(temp);

  temp = _tempSec * (uint32_t)1000 + temp;
  return temp;
}

uint32_t second()
{
  return _second;
}

void updateSec(uint32_t sec)
{
  //check if input sec is different from existing sec.
  _second = sec;
}


void rtcBegin()
{
  if (! RTC.isRunning())
  {
    Serial.println(F("RTC Not Found"));
  }
  else
  {
    Serial.println(F("RTC Found"));
  }
  if (!RTC.isRunning())
  {
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println(F("RTC Adjusted"));
  }
  else
  {
    // Serial.println(F("-->RTC is Running"));
  }
}

uint32_t rtcGetSec()
{
  uint32_t utime = 0;
  if (RTC.isRunning())
  {
    Serial.println(F("Getting RTC time"));
    // DateTime now = rtc.now();
    // utime =  now.unixtime();
    utime = RTC.get();
  }
  return utime;
}

void rtcUpdateSec(uint32_t unix)
{
  if (RTC.isRunning())
  {
    uint32_t rtcSec = rtcGetSec();
    Serial.print(F("RTC Sec: ")); Serial.println(rtcSec);
    if (abs((int32_t)(unix - rtcSec)) > 1)
    {
      //if difference is more than 1 sec, update rtc
      Serial.println(F("Updating RTC Time"));
      // rtc.adjust(DateTime(unix));
      RTC.set(unix);
    }
    else
    {
      Serial.println(F("RTC is updated"));
    }
  }
}


bool rtcIsRunning()
{
  return RTC.isRunning();
}

uint32_t calcNextSlotUnix(uint32_t uSec, nrfNodeConfig_t *conf)
{
  // Serial.print("uSec: ");Serial.println(uSec);

  uint16_t slotSec = (conf -> perNodeInterval) * (conf -> slotId);
  // Serial.print("slotSec: ");Serial.println(slotSec);
  uint16_t curMoment = uSec % conf->momentDuration;
  // Serial.print("momentDuration: ");Serial.println(conf->momentDuration);

  uint32_t nexSlotSec;
  if (curMoment < slotSec)
  {
    nexSlotSec = uSec + (slotSec - curMoment);
    // Serial.println("..");
  }
  else
  {
    // Serial.println("...");
    nexSlotSec = uSec + (conf->momentDuration - curMoment) + slotSec;
  }
  // Serial.print(F("curMoment :")); Serial.println(curMoment);
  Serial.print(F("===>>nextSlot:")); Serial.println(nexSlotSec);
  return nexSlotSec;
}

// uint32_t getNtpTime()
// {
//   uint32_t utime[3];
//   for (uint8_t i = 0; i < 3; i++)
//   {
//     utime[i] = clientGetNtpTime();
//     Serial.println(utime[i]);
//   }
//   uint32_t unixtm = majorityElement(utime, sizeof(utime) / sizeof(utime[0]));
//   Serial.print(F("Majority time: "));
//   Serial.println(unixtm);
//   return unixtm;
// }
