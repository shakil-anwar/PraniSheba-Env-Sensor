#include "time.h"
#include "RTClib.h"
#include "AVR_Timer1.h"

#define SIX_HOUR_SECOND (6*3600)

RTC_DS1307 rtc;
void timerIsr(void);
uint32_t sec = 0;

void timerIsr(void)
{
  sec++;
  Serial.println(F("Timer ISR Triggered"));
}

void timeBegin()
{
  timer1.initialize(1);
  timer1.attachIntCompB(timerIsr);
  sec = getUnixTime() - SIX_HOUR_SECOND;
  timer1.start();
}
void rtcBegin()
{
  if (! rtc.begin())
  {
    Serial.println(F("RTC Not Found"));
  }
  if (!rtc.isrunning())
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println(F("RTC Adjusted"));
  }
}

uint32_t second()
{
  return sec;
}


void setSecond(uint32_t second)
{
  sec = second;
}

uint32_t getUnixTime()
{
  DateTime now = rtc.now();
  return now.unixtime();
}

void updateTime(uint32_t unixTime)
{
  sec = unixTime - SIX_HOUR_SECOND;
  rtc.adjust(DateTime(unixTime));
}

void printDateTime(uint32_t unixTime)
{
  DateTime dt(unixTime);
  char buf4[] = "DD/MM/YYYY-hh:mm:ss";
  Serial.println(dt.toString(buf4));
}
