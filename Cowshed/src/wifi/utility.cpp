#include "utility.h"

#if defined(PROD_BUILD)
#include "./lib/NTPClient/NTPClient.h"
#else
#include <NTPClient.h>
#endif



#include <WiFiUdp.h>
#include "param.h"
#include "wifi.h"


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.google.com", 0, 60000); //


void wifiParamBegin()
{
#if defined(GSM_WIFI)
  Serial.println("gms + wifi");
  netState.deviceHasGsm = true;
  netState.deviceHasWifi = true;
#elif defined(GSM_V010)
  netState.deviceHasGsm = true;
  netState.deviceHasWifi = false;
#else
  netState.deviceHasWifi = true;
  netState.deviceHasGsm = false;
#endif

  if(netState.deviceHasWifi)
  {
    netState.networkType = WIFI_STATE;
  }
  else if(netState.deviceHasGsm)
  {
    netState.networkType = GSM_STATE;
  }
}

void ntpBegin()
{
  timeClient.begin();
}
void ntpUpdateLoop()
{
  if(wifiIsconnected())
  {
    timeClient.update();
  }
  
}

void printNtpTime()
{
  debugSerial.print(F("NTP Time: "));debugSerial.println(timeClient.getFormattedTime());
  debugSerial.println(F("Unix Time: "));debugSerial.println(timeClient.getEpochTime());
}

uint32_t getUnixTime()
{  
  return timeClient.getEpochTime();
}

void print_pointer(void *ptr)
{
  char buffer[10];
  sprintf(buffer, "%p", ptr);
  debugSerial.println(buffer);
}


void digitalToggle(uint8_t pin )
{
  bool status = digitalRead(pin);
  digitalWrite(pin, !status);
}

char  *trim(char *str)
{
  /*
     The function returns the pointer of the input parameter
     deletting start and end white space
  */
  while (isspace(*str))
  {
    str++;
  }

  int len = strlen(str);
  char *end;//end pointer;
  end = str + len - 1;

  while (isspace(*end) && end > str)
  {
    end--;
  }
  end[1] = '\0'; //Null terminate

  return str;
}
