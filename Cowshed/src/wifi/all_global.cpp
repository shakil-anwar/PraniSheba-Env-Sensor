#include "all_global.h"

#define PROCESSTIMOUT 50

extern "C"
{
#include <lwip/icmp.h> // needed for icmp packet definitions
}




char buf[1024];
uint32_t prevProcessMillis;
uint8_t ringQBuff[RINGQ_BUFFER_LEN];
struct ringq_t ringqObj1;
struct ringq_t *ringqObj;

qObj_t *tailObj;
uint8_t pipeMid;

volatile uint8_t midSendFlag = 0;


#if defined(ESP32_V010)
// bool checkQos2Ack(uint8_t mid);
uint32_t getEspNtpTime(void);
#endif



void all_setup(void)
{
  wifiParamBegin();   // This parameter must be began first
#if defined(WDT_PIN)
  espWdtBegin(WDT_PIN);
#endif
#if !defined(GSM_V010)
  WiFi.mode(WIFI_STA);
  // WiFi.begin();
  delay(2000);
#endif
  // pinMode(SERVER_PIN, OUTPUT);
  // digitalWrite(SERVER_PIN, LOW);
//  Serial1.begin(115200);
  
  debugSerial.begin(DEBUG_SERIAL_SPEED);
  debugSerial.println("esp started");
#if defined(ESP8266)
  unoSerial.begin(PIPE_SERIAL_SPEED);
  unoSerial.flush();
#endif 

#if defined(OTA)
  otaBegin();
#endif //OTA

  prevProcessMillis = 0;
  iotClientBegin();
  
  if (netState.deviceHasWifi)
  {
    ntpBegin();
  }
  
  ringqObj1._baseAddr = &ringQBuff[0];
  ringqObj1._len = RINGQ_BUFFER_LEN;
  ringqObj1._endAddr = (uint8_t*)&ringQBuff[0] + RINGQ_BUFFER_LEN;
  ringqObj = &ringqObj1;
  ringqReset(ringqObj);
  tailObj = NULL;
  // attachQos2AckFunc((bool (*)(uint8_t)));

#if defined(ESP32_V010)
  attachQos2AckFunc(checkQos2Ack);
  attchMqttFunc(mqttIsConnected);
  attachNtpFunc(getEspNtpTime);
  attachDataSendToServer(espDataSendToServer);
  if (netState.networkType == WIFI_STATE)
  {
    Serial.println("wifi func attach");
    attchWifiConnect(wifiIsconnected);
    pubSubPtr = &mqtt;
  }
  else if(netState.networkType == GSM_STATE)
  {
    Serial.println("gprs func attach");
    attchWifiConnect(gprsConnStatus);
    pubSubPtr = &simMqtt;    
  }
#endif
#if defined(WDT_PIN)
  espWdtReset(true);
#endif
  debugSerial.println("Main Setup Done");
  delay(100);
  debugSerial.println("BS_WiFi _v0.8.0");
}

#if defined(ESP32_V010)
bool checkQos2Ack(uint8_t mid)
{
  // debugSerial.println("ack qos2");
  return pubSubPtr->isqos2ack(mid);
}

uint32_t getEspNtpTime(void)
{
  static uint32_t lastNtpCallTime = 0;
  static uint32_t utime = 0;
  if(netState.networkType == WIFI_STATE)
  {
    // uint32_t utime = 0;
    if (wifiIsconnected())
    {
      utime = getUnixTime();      
      debugSerial.print(F("uTime : ")); debugSerial.println(utime);
    }
    return utime;
  }
  else if(netState.networkType == GSM_STATE)
  {
    // static uint32_t lastNtpCallTime = 0;
    // static uint32_t utime = 0;
    if(((uint32_t) millis()- lastNtpCallTime)> 10000)
    {
      utime = 0;
      // utime = getGprsNtpTime();
      utime = getGprsUnixTime();
      lastNtpCallTime = millis();
    }

    if(utime)
    {
      lastNtpCallTime = millis();
      return utime;
    }
    else
    {
      return (uint32_t)0UL;
    }
  }
  return utime;
}

#endif


void espDataSendLoopMid()
{
  if(mqttIsConnected())
  {
    mqtt.loop();
    if((!mqtt.qos2Full()) && (millis() - prevProcessMillis >= PROCESSTIMOUT))
    {
      tailObj = ringqNextTail(&ringqObj1);
      //debugSerial.print("Tail Object: ");
      //debugSerial.println((uint32_t)tailObj);
      if(tailObj)
      {
  //      memset(buf, '\0', 300);
  //      memcpy(buf, (tailObj ->ptr + sizeof(struct qObj_t)), tailObj->len);
        uint8_t *payloadPtr = tailObj ->ptr + sizeof(struct qObj_t);
        uint8_t mid = *payloadPtr; // first byte of the data buffer is mid.
        payloadPtr++;
        uint16_t payloadLen = tailObj->len-1;
        debugSerial.print("Rcvd Mid: "); debugSerial.println(mid);
        
        if(payloadPtr[2]=='2' && payloadPtr[3] >= '0' && payloadPtr[3] <= '9')
        {
          if(pubSubPtr->publish_Q1(log_topic,payloadPtr,payloadLen,mid))
          {
            midSendFlag = mid;
            ringqPop(&ringqObj1);
          }
        }
        else
        { 
          if(pubSubPtr == NULL)
          {
            Serial.println("pubSubPtr null");
          }
          else
          {
            if(pubSubPtr->publish_Q1(payload_topic,payloadPtr,payloadLen,mid))
            {
              midSendFlag = mid;
              ringqPop(&ringqObj1);
              memcpy(buf, payloadPtr, payloadLen);
              debugSerial.print("Sent: "); debugSerial.println(buf);
            }
          }
        }
      }
      prevProcessMillis = millis();
    }
    pubSubPtr->loop();
  }   
}

bool espDataSendToServer(void *buffer, uint16_t len)
{
  if(mqttIsConnected())
  {
    pubSubPtr->loop();
    if((!mqtt.qos2Full()) && (millis() - prevProcessMillis >= PROCESSTIMOUT))
    {
      uint8_t *payloadPtr = (uint8_t*)buffer;
      uint8_t mid = *payloadPtr; // first byte of the data buffer is mid.
      payloadPtr++;
      uint16_t payloadLen = len-1;
      debugSerial.print("Sending Mid: "); debugSerial.println(mid);
      
      if(payloadPtr[2]=='2' && payloadPtr[3] >= '0' && payloadPtr[3] <= '9')
      {
        if(pubSubPtr->publish_Q1(log_topic,payloadPtr,payloadLen,mid))
        {
          debugSerial.print("Sending to server>>");
          return true;
        }
      }
      else
      { 
        if(pubSubPtr == NULL)
        {
          Serial.println("pubSubPtr null");
        }
        else
        {
          if(pubSubPtr->publish_Q1(payload_topic,payloadPtr,payloadLen,mid))
          {
            debugSerial.print("Sending to server>>");
            memcpy(buf, payloadPtr, payloadLen);
            debugSerial.print("Sent: "); debugSerial.println(buf);
            return true;
          }
        }
      }
      prevProcessMillis = millis();
    }
    pubSubPtr->loop();
    return false;
  }
  else 
  {
    return false;
  }   
}

void mqttLoop()
{
  if(netState.dataSendReady == 1)
  {
    if(mqttIsConnected())
    {
      pubSubPtr->loop();
    }
  }
}