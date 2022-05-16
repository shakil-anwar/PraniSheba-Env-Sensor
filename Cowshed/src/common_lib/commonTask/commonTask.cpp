#include "commonTask.h"

uint32_t (*_getNtpFunc) (void) = NULL;
bool (*_wifiConnectFunc) (void) = NULL;
bool (*_mqttAvailFunc) (void) = NULL;
bool (*_dataSendToServerFunc)(void *buffer, uint16_t len) = NULL;
bool (*_isMqttAck)(uint8_t) = NULL;
uint8_t _wdtEspPin;




void attachNtpFunc(uint32_t (*getNtpFunc)(void))
{
    _getNtpFunc = getNtpFunc;
}

uint32_t getNtp(void)
{
    if(_getNtpFunc)
    {
        return _getNtpFunc(); 
    }
    return 0;
}

void attchWifiConnect(bool (*wifiConnectFunc)(void))
{
    _wifiConnectFunc = wifiConnectFunc;
}
bool isWifiAvailable()
{
    if(_wifiConnectFunc)
    {
        return _wifiConnectFunc();
    }
    return false;
}

uint16_t calcLen(uint8_t *buf)
{
    uint16_t len = 0; 
    char *dataBuff = (char *)buf;
    while(*dataBuff)
    {
        len++;
        dataBuff++;
    }
    return len;
}


void attchMqttFunc( bool (*mqttAvailFunc)(void))
{
    _mqttAvailFunc = mqttAvailFunc;

}
bool isMqttOk()
{
    if(_mqttAvailFunc)
    {
        return _mqttAvailFunc();
    }
    return false;
}

void attachDataSendToServer( bool(*dataSendToServerFunc)(void *buffer, uint16_t len))
{
    _dataSendToServerFunc = dataSendToServerFunc;
}

bool dataSendToServer(void *buffer, uint16_t len)
{   
    if (_dataSendToServerFunc)
    {
        return _dataSendToServerFunc(buffer, len);
    }
    return false;
}

/******* to check QOS2 ack******/
void attachQos2AckFunc(bool (*mqttQosSendOkay)(uint8_t))
{
	_isMqttAck = mqttQosSendOkay;
}

bool checkmqttQos2ack(uint8_t mid)
{
	if(_isMqttAck)	
	{
		return _isMqttAck(mid);
	}
	return false;
}

void espWdtBegin(uint8_t wdtPin)
{
  _wdtEspPin = wdtPin;
  pinMode(_wdtEspPin, OUTPUT);
  espWdtReset(true);
}

void espWdtReset(bool bResetFlag)
{
  static uint32_t millisEspReset = 0;
  if (millis() - millisEspReset > 5000 || bResetFlag)
  {
    // pinMode(_wdtEspPin, OUTPUT);
    // digitalWrite(_wdtEspPin,LOW);
    // delay(1); 
    // pinMode(_wdtEspPin, INPUT);

    digitalWrite(_wdtEspPin, HIGH);
    delay(1); 
    digitalWrite(_wdtEspPin, LOW);
    
    millisEspReset = millis();
    Serial.println("wdt>>reset");
  }
  
}