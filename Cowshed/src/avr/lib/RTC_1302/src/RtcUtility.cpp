
#include <Arduino.h>
#include "RtcUtility.h"
#include <time.h>

uint8_t BcdToUint8(uint8_t val)
{
    return val - 6 * (val >> 4);
}

uint8_t Uint8ToBcd(uint8_t val)
{
    return val + 6 * (val / 10);
}

uint8_t BcdToBin24Hour(uint8_t bcdHour)
{
    uint8_t hour;
    if (bcdHour & 0x40)
    {
        // 12 hour mode, convert to 24
        bool isPm = ((bcdHour & 0x20) != 0);

        hour = BcdToUint8(bcdHour & 0x1f);
        if (isPm)
        {
           hour += 12;
        }
    }
    else
    {
        hour = BcdToUint8(bcdHour);
    }
    return hour;
}



// 

RtcDateTime unix2dateTime(uint32_t unixtime)
{
    time_t time= (time_t)unixtime;
    struct tm t;
    t = *localtime(&time);
    return RtcDateTime(t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour,  t.tm_min, t.tm_sec);
}



uint32_t dateTime2unix(const RtcDateTime& dt)
{   
   struct tm t;
   time_t t_of_day;
  
   t.tm_year= dt.Year()-1900;
  t.tm_mon= dt.Month()-1;
  t.tm_mday = dt.Day();
  t.tm_hour= dt.Hour();
  t.tm_min=dt.Minute();
  t.tm_sec= dt.Second();
  t.tm_isdst = 0;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
  t_of_day = mktime(&t);
  // Serial.println(t_of_day);

  return (uint32_t) t_of_day;
}