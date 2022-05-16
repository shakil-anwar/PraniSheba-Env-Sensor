#include "iotClient.h"
#include "wifi.h"
#include "mqtt.h"
#include "param.h"
#include "pin.h"
#include "utility.h"
#include "smart_config.h"
#include "all_global.h"




#define IOT_CONN_CHECK_MS  10000
#define IOT_SOFT_AP_MS     20000
#define MQTT_RE_CONN_DELAY 20000

String _espSsid       = "";
String _espPass       = "";

typedef enum 
{
  WIFI_SETUP,
  WIFI_BEGIN,
  WIFI_CONNECT,
  WIFI_MQTT_CONNECT,
  WIFI_MQTT_RETRY_WAIT,
  WIFI_RUN,
  SOFT_AP_START,
  SOFT_AP_END,
  WIFI_IDLE,
}wifiState_t;

typedef enum 
{
  GSM_SETUP,
  GSM_BEGIN,
  GSM_CONNECT,
  GPRS_CONNECT,
  GSM_MQTT_CONNECT,
  GSM_RUN,
  GSM_WAIT,
  GSM_EXIT,
  GSM_IDLE,
}gsmState_t;


void printIoTClientState();

/*********IoT client Variable*************/
wifiState_t wifiState;
wifiState_t prevWifiState;
gsmState_t  gsmState;
gsmState_t prevGsmState;
espIotState_t espIotState;
espIotState_t prevEspIotState;
uint32_t iotPrevMs;
uint32_t pingTime;
uint32_t mqttRetTime;
static uint32_t iotLoopTime;
bool pingStatus;
uint32_t mqttPrevMS;

bool startUpStatus;


void iotClientBegin()
{
  // wifiBegin();
  if(netState.deviceHasWifi)
  {
    wifiBegin(_espSsid,_espPass);
    mqttBegin();
  }

  if(netState.deviceHasGsm)
  {
    simSetup();
  }
  setupSmartConfig((char*)BASE_STATION_ID, SMART_CONFIG_TIMEOUT);

  iotPrevMs = 0;
  pingTime = 0;
  mqttRetTime = 0;
  iotLoopTime = 0;
  pingStatus = false;

  //initialize wifi state
  wifiState = WIFI_BEGIN;
  prevWifiState = wifiState;

  //initialize gsm state
  gsmState  = GSM_BEGIN;
  prevGsmState = gsmState;

  // initialize netstates
  netState.internetAvail = false;
  netState.networkAvail = false;
  netState.mqttStatus = 0;
  netState.ntpTime = 0;

  startUpStatus = false;
}

void iotConnect()
{
  wifiState = WIFI_MQTT_CONNECT;
}


void espIoTClientLoop()
{
  switch(espIotState)
  {
    case IOT_BEGIN:
      if(netState.deviceHasWifi)
      {
        iotLoopTime = (uint32_t)millis();
        netState.networkType = WIFI_STATE;
        wifiState = WIFI_BEGIN;
      }
      else if(netState.deviceHasGsm)
      {
        iotLoopTime = (uint32_t)millis();
        netState.networkType = GSM_STATE;
        gsmState = GSM_BEGIN;
      }
      espIotState = IOT_CONNECT;
      break;
    case IOT_CONNECT:
      if (netState.networkType == WIFI_STATE)
      {
        espIotState = IOT_RUN_WIFI;
        attchWifiConnect(wifiIsconnected);
        pubSubPtr = &mqtt;
      }
      else if(netState.networkType == GSM_STATE)
      {
        espIotState = IOT_RUN_GSM;
        attchWifiConnect(gprsConnStatus);
        pubSubPtr = &simMqtt;    
      }
      break;
    case IOT_RUN_WIFI:
      if(((uint32_t)millis()- iotLoopTime) > 90000)
      {
        if(!wifiIsconnected() && (wifiState == SOFT_AP_START))
        {
          if(netState.deviceHasGsm)
          {
            netState.networkType = GSM_STATE;
            espIotState = IOT_RUN_GSM;
            gsmState = GSM_BEGIN;
            pubSubPtr = &simMqtt;
          }
        }
        iotLoopTime = (uint32_t)millis();
      }
      wifiClientLoop();
      ntpUpdateLoop();
      break;
    case IOT_RUN_GSM:
      
      if(netState.deviceHasWifi)
      {
        wifiClientLoop();
        if(((uint32_t)millis()- iotLoopTime) > 90000)
        {
          if(wifiIsconnected())
          {
            netState.networkType = WIFI_STATE;
            espIotState = IOT_RUN_WIFI;
            wifiState = WIFI_CONNECT;
            pubSubPtr = &mqtt;
          }        
          iotLoopTime = (uint32_t)millis();
        }
      }
      gsmClientLoop();
      break;
    case IOT_IDLE:
      break;
  }
  printIoTClientState();
}


void wifiClientLoop()
{
  static volatile uint8_t count = 0;
  // static voltaile uint32_t wifiWaitTime = 0 ;
  // bool googlePing;
  int err;
  // IPAddress result; wifiState == SOFT_AP_START
  switch (wifiState)
  {
    case WIFI_BEGIN:
      debugSerial.println(F("wifiState :WIFI_BEGIN"));
      wifiBegin("","");
      wifiState = WIFI_CONNECT;
      break;

    case WIFI_CONNECT:
      if(prevWifiState != wifiState)
      {
        debugSerial.println(F("wifiState :WIFI_CONNECT"));
        prevWifiState = wifiState;
        iotPrevMs = millis();
        wifiConnect();
      }
      if((millis()- iotPrevMs)< 60000)
      {
        if(wifiConnect())
        {
          delay(200);
          netState.networkAvail = true;
          if(Ping.ping("www.google.com",4))
          {
            debugSerial.println("ping successful");
            pingStatus = true;
            netState.internetAvail = true;
            wifiState = WIFI_MQTT_CONNECT;
            startCalibServer();
            iotPrevMs = millis();
          }
          else
          {
            netState.internetAvail = false;
            pingStatus = false;
            debugSerial.println("ping failed");
          }
        }
        else
        {
          netState.internetAvail = false;
          netState.networkAvail = false;
        }
      }
      else
      {
        iotPrevMs = millis();
        
        _espSsid = WiFi.SSID();
        debugSerial.print(F("SSID: "));
        debugSerial.println(_espSsid);
        _espPass = WiFi.psk();
        debugSerial.print("Password: ");
        debugSerial.println(_espPass);
        softApState = SMART_CONFIG_START;
        wifiState = SOFT_AP_START;
      }
      break;
    case WIFI_MQTT_CONNECT:
      if(prevWifiState != wifiState)
      {
        debugSerial.println(F("wifiState :WIFI_MQTT_CONNECT"));
        prevWifiState = wifiState;
        iotPrevMs = millis();
      }
      
      if((millis() - iotPrevMs)>IOT_CONN_CHECK_MS)
      {
        delay(200);
        if(mqttConnect())
        {
          netState.mqttStatus = 1;
          netState.dataSendReady = 1;
          wifiState = WIFI_RUN;
          iotPrevMs = 0;
          debugSerial.println(F("Going to Run"));
        }
        else
        {
          netState.mqttStatus = 0;
          wifiState = WIFI_CONNECT;
          iotPrevMs = 0;
        }
        iotPrevMs = millis();
      }
      break;
    case WIFI_RUN:
      if(prevWifiState != wifiState)
      {
        debugSerial.println(F("wifiState :WIFI_RUN"));
        prevWifiState = wifiState;
        iotPrevMs = millis();
      }
      if (millis() - iotPrevMs > IOT_CONN_CHECK_MS)
      {
        iotPrevMs = millis();
        if (wifiIsconnected())
        {
          wifiLedHigh();
          if (mqttIsConnected())
          {
            netState.mqttStatus = 1;
            debugSerial.println(F("<WIFI & MQTT Connected>"));
          }
          else
          {
            netState.mqttStatus = 0;
            debugSerial.print("MQTT STATE: ");
            debugSerial.println(mqtt.state());
            debugSerial.println(F("<MQTT Conn Lost>"));
            wifiState = WIFI_MQTT_CONNECT;
            iotPrevMs = 0;
          }
        }
        else
        {
          netState.mqttStatus = 0;
          netState.internetAvail = false;
          netState.networkAvail = false;
          debugSerial.println(F("<WIFI & MQTT Conn Lost>"));
          wifiState = WIFI_MQTT_CONNECT;
          iotPrevMs = 0;
        }
      }
      break;
    case SOFT_AP_START:
      if(prevWifiState != wifiState)
      {
        debugSerial.println(F("wifiState :SOFT_AP_START"));
        prevWifiState = wifiState;
      }
      handleSmartConfig();
      if(softApState == SMART_CONFIG_IDLE)
      {
//        debugSerial.println(F("Going to SOFT_AP_END"));
        if(!wifiIsconnected())
        {
#if defined(ESP32_V010)
          // char wifiName[_espSsid.length()];
          // char wifiPass[_espPass.length()];
          // _espSsid.toCharArray(wifiName, _espSsid.length());
          // _espPass.toCharArray(wifiPass, _espPass.length());
          // WiFi.begin(wifiName, wifiPass);
          WiFi.begin(_espSsid.c_str(),_espPass.c_str());
#else
          _espSsid = WiFi.SSID();
          debugSerial.print(F("SSID: "));
          debugSerial.println(_espSsid);
          _espPass = WiFi.psk();
          debugSerial.print("Password: ");
          debugSerial.println(_espPass);
          WiFi.begin(_espSsid, _espPass);
#endif
          delay(1000);
        }
        wifiState = SOFT_AP_END;
      }
      break;
    case SOFT_AP_END:
      debugSerial.println(F("wifiState :SOFT_AP_END"));
        wifiState = WIFI_CONNECT;
        iotPrevMs = millis();
      break;
  }
  if(wifiIsconnected())
  {
    runCalibServer();
  }
}

void gsmClientLoop()
{
  static volatile uint8_t count = 0;
  // static voltaile uint32_t wifiWaitTime = 0 ;
  // bool googlePing;
  int err;
  // IPAddress result; gsmState
  switch (gsmState)
  {
    case GSM_BEGIN:
      if(prevGsmState != gsmState)
      {
        iotPrevMs = millis();
        debugSerial.println(F("gsmState :GSM_BEGIN"));
        prevGsmState = gsmState;

      }

      if(((millis() -iotPrevMs)> 30000) || (startUpStatus == false))
      {
        iotPrevMs = millis();
        
        // simSetup();
        // if(startUpStatus == false)
        // {
          simBegin(); // need to update with auto retry;
        // }
        startUpStatus = true;
              

        if(isGprsAvailable())
        {
          netState.networkAvail = true;
          gsmState = GSM_CONNECT;
        }
        else
        {
          debugSerial.println(F("gsm network not available"));
          netState.mqttStatus = 0;
          netState.internetAvail = false;
          netState.networkAvail = false;
        }        
      }
      break;

    case GSM_CONNECT:
      if(prevGsmState != gsmState)
      {
        debugSerial.println(F("gsmState :GSM_CONNECT"));
        prevGsmState = gsmState;
        iotPrevMs = millis();
      }
      if(isGprsConnected())
      {
        delay(200);
        if(getGprsNtpTime())
        {
          debugSerial.println("ping successful");
          netState.internetAvail = true;
          netState.networkAvail = true;
          pingStatus = true;
          gsmState = GSM_MQTT_CONNECT;
          iotPrevMs = millis();
        }
        else
        {
          netState.mqttStatus = 0;
          netState.internetAvail = false;
          pingStatus = false;
          debugSerial.println("ping failed");
        }
      }
      // delay(10000);
      delay(200);
      break;
    case GSM_MQTT_CONNECT:
      if(prevGsmState != gsmState)
      {
        debugSerial.println(F("gsmState :GSM_MQTT_CONNECT"));
        prevGsmState = gsmState;
        iotPrevMs = millis();
      }
      
      if((millis() - iotPrevMs)>IOT_CONN_CHECK_MS)
      {
        delay(200);
        if(simMqttConnect())
        {
          netState.mqttStatus = 1;
          gsmState = GSM_RUN;
          iotPrevMs = 0;
          debugSerial.println(F("Going to Run"));
        }
        else
        {
          netState.mqttStatus = 0;
          gsmState = GSM_CONNECT;
          iotPrevMs = 0;
        }
        iotPrevMs = millis();
      }
      break;
    case GSM_RUN:
      if(prevGsmState != gsmState)
      {
        debugSerial.println(F("gsmState :GSM_RUN"));
        prevGsmState = gsmState;
        iotPrevMs = millis();
      }
      if (millis() - iotPrevMs > IOT_CONN_CHECK_MS)
      {
        iotPrevMs = millis();
        if (netState.mqttStatus)
        {
          wifiLedHigh();
          if (mqttIsConnected())
          {
            netState.mqttStatus = 1;
            netState.dataSendReady = 1;
            debugSerial.println(F("<GSM & MQTT Connected>"));
          }
          else
          {
            netState.mqttStatus = 0;
            debugSerial.print("MQTT STATE: ");
            debugSerial.println(simMqtt.state());
            debugSerial.println(F("<MQTT Conn Lost>"));
            gsmState = GSM_MQTT_CONNECT;
            iotPrevMs = 0;
          }
        }
        else
        {
          netState.mqttStatus = 0;
          netState.internetAvail = false;
          netState.networkAvail = false;
          debugSerial.println(F("<GSM & MQTT Conn Lost>"));
          gsmState = GSM_CONNECT;
          iotPrevMs = 0;
        }
      }
      break;
  }
}

void iotClientLoopV2()
{

}



bool gprsConnStatus()
{
  if(netState.mqttStatus)
  {
    delay(1500);
    return true;
  }
  return false;
}


void printIoTClientState()
{
  static espIotState_t lastEspIotState = IOT_IDLE;

  if(lastEspIotState != espIotState)
  {
    switch(espIotState)
    {
      case IOT_BEGIN:
        debugSerial.println(F("espIotState :IOT_BEGIN"));
        break;
      case IOT_RUN_WIFI:
        debugSerial.println(F("espIotState :IOT_RUN_WIFI"));
        break;
      case IOT_RUN_GSM:
        debugSerial.println(F("espIotState :IOT_RUN_GSM"));
        break;
      case IOT_IDLE:
        debugSerial.println(F("espIotState :IOT_IDLE"));
        break;
    }
    lastEspIotState = espIotState;
  }
}