#include "Timers.h"

#include "radio.h"
//#include "RTClib.h"
//#include "AVR_Timer1.h"

#if defined(ESP32_V010) && defined(SOFTWARE_I2C)
ThreeWire myWire(DS1302_IO,DS1302_SCLK,DS1302_CE); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
#else
ErriezDS1307 RTC;
#endif

#define YEAR_2000_UNIX_SEC 946684800UL

uint32_t getNtpTime();

volatile uint32_t _second;
volatile uint32_t _pre_second;
volatile uint32_t _tempSec;
volatile uint32_t _nextSlotSec;
volatile bool _readyToSend;

#if defined(ESP32_V010)
// portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t * timer = NULL;
void timer1Start1();
void IRAM_ATTR onTimerHandler() 
{
  // portENTER_CRITICAL_ISR(&mux);
  _second++;
  _tempSec++;
  // Serial.println(_second);
  // portEXIT_CRITICAL_ISR(&mux);

}


#else
void timer1Start();
void timerIsr(void);
#endif
void updateSec(uint32_t sec);

void printSecond()
{
    if(_second!=_pre_second)
  {
    // Serial.print("sec: "); 
    Serial.println(_second);
    _pre_second = _second;
  }
}



// RTC_DS1307 rtc;

void TimersBegin()
{
   _readyToSend = false;
  _second = 0;
  _tempSec = 0;
  #if defined(ARDUINO_ARCH_ESP32)
  // Prescaler 80 is for 80MHz clock. One tick of the timer is 1us
  timer = timerBegin(0, 80, true); 
  
  //Set the handler for the timer
  timerAttachInterrupt(timer, &onTimerHandler, true); 

  // How often handler should be triggered? 1000 means every 1000 ticks, 1ms
  //1000,000 1s
  timerAlarmWrite(timer, 1000000, true); 

  //And enable the timer
  timerAlarmEnable(timer);
  #else
  timer1.initialize(1);
  timer1.attachIntCompB(timerIsr);
  #endif

  rtcBegin();

  
  rtAttachRTC(rtcGetSec, rtcUpdateSec);
  #if defined(ESP32_V010)
  rtAttachFastRTC(second, updateSec, timer1Start1);
  #else
  rtAttachFastRTC(second, updateSec, timer1Start);
  #endif
  rtBegin();
}

#if defined(ARDUINO_ARCH_ESP32)
void timer1Start1()
{
   
}




#else
void timerIsr(void)
{
  _second++;
  _tempSec++;
  // Serial.println(_second);
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
#endif

uint32_t ms()
{
  #if defined(ARDUINO_ARCH_ESP32)
   return 1111;
  #else
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
  #endif
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
  #if defined(ESP32_V010)  && defined(SOFTWARE_I2C)
  Rtc.Begin();
  /////////////////
  // RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  // uint32_t unix;
  // unix = dateTime2unix(compiled);
  // Serial.println(unix);  
  // // Rtc.SetDateTime(compiled);
  // Rtc.setUnixTime(unix);
  // Serial.println("[set]");
  
  // unix = 0;
  // unix = Rtc.getUnixTime();
  // Serial.print("[get]");
  // Serial.print("...unix: "); Serial.println(unix);
  // updateSec(unix);
  
  //////////////
  if(Rtc.GetIsWriteProtected())
  {
      Serial.println("RTC was write protected, enabling writing now");
      Rtc.SetIsWriteProtected(false);
  }
  if (!Rtc.GetIsRunning())
  {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
  }
  
  if(! Rtc.GetIsRunning())
  {
    Serial.println(F("RTC Not Found"));
  }    
  else
  {
    Serial.println(F("RTC Found"));
  }


  
  #else

  Wire.begin();
  Wire.setClock(100000);

  if (!RTC.isRunning())
  {
    RTC.clockEnable(true);
    Serial.println(F("DS1307 RTC Not Found"));
  }
  else
  {
    Serial.println(F("RTC Found"));
  }

  #endif
}

uint32_t rtcGetSec()
{
  uint32_t utime = 0;
  #if defined(ESP32_V010)  && defined(SOFTWARE_I2C)
  // Rtc.Begin();
  if(Rtc.GetIsRunning()) 
  {
    Serial.print(F("Getting RTC time : "));
    utime = Rtc.getUnixTime();
    Serial.println(utime);
  }  
  #else
  if (RTC.isRunning())
  {
    Serial.print(F("Getting RTC time: "));
    // DateTime now = rtc.now();
    // utime =  now.unixtime();
    // utime = RTC.get();
    utime = RTC.getEpoch();
    Serial.println(utime);
  }
  #endif
  return utime;
}

void rtcUpdateSec(uint32_t unix)
{
  #if defined(ESP32_V010) && defined(SOFTWARE_I2C)
  // Rtc.Begin();
  if(Rtc.GetIsRunning())
  #else
  if (RTC.isRunning())
  #endif
  {
    uint32_t rtcSec = rtcGetSec();
    Serial.print(F("RTC Sec: ")); Serial.println(rtcSec);
    if (abs((int32_t)(unix - rtcSec)) > 1)
    {
      //if difference is more than 1 sec, update rtc
      Serial.println(F("Updating RTC Time"));
      // rtc.adjust(DateTime(unix));
      #if defined(ESP32_V010)  && defined(SOFTWARE_I2C)
      Rtc.setUnixTime(unix);
      #else
      // RTC.set(unix);
      RTC.setEpoch(unix);
      #endif
    }
    else
    {
      Serial.println(F("RTC is updated"));
    }
  }
}


bool rtcIsRunning()
{
  #if defined(ESP32_V010) && defined(SOFTWARE_I2C)
  // Rtc.Begin();
  bool f =Rtc.GetIsRunning();
  return f;
  // return Rtc.GetIsRunning();
  
  #else
  return RTC.isRunning();
  #endif
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
    nexSlotSec = uSec + (conf->momentDuration - curMoment) + slotSec;
  }
  // Serial.print(F("curMoment :")); Serial.println(curMoment);
  Serial.print(F("===>>nextSlot:")); Serial.println(nexSlotSec);
  return nexSlotSec;
}

#if defined(ESP32_V010)
uint32_t getNtpTime()
{
  uint32_t unixtm = 0;
  // if (isWifiAvailable())
  if (netState.internetAvail)
  {
    uint32_t utime[3];
    for (uint8_t i = 0; i < 3; i++)
    {
      utime[i] = getNtp();
  
      // Serial.print(F("NTP Time : ")); Serial.println(utime[i]);
    }
    
    unixtm = majorityElement((unsigned long*)utime, sizeof(utime) / sizeof(utime[0]));
    if(unixtm < YEAR_2000_UNIX_SEC)
    {
      unixtm = 0;
      Serial.println(F("NTP Invalid"));
    }
    Serial.print(F("NTP Time: ")); Serial.println(unixtm);
  }
  // Serial.print(F("NTP Time: ")); Serial.println(unixtm);

  return unixtm;

}

#endif