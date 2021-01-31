#include "asyncServer.h"



AsyncServer::AsyncServer(MemQ *memQPtr)
{
	_memQPtr = memQPtr;
}

void AsyncServer::setServerCbs(send_t send, ackWait_t ackFunc)
{
  _send = send;
  _ackWait = ackFunc;
}

void AsyncServer::setServerCbs(sendL_t send, ackWait_t ackFunc)
{
  _sendL = send;
  _ackWait = ackFunc;
}

void AsyncServer::setSchema(uint8_t payloadSize, uint8_t total)
{
  _payloadSz = payloadSize;
  _payloadTotal = total;
  Serial.print(F("Pld Size: ")); Serial.println(payloadSize);
  // payloadBuf = (uint8_t*)malloc(payloadSize * _payloadTotal);
  payloadBuf = (uint8_t*)calloc(payloadSize * _payloadTotal,sizeof(uint8_t));
  if (payloadBuf != NULL)
  {
    Serial.println(F("Payload Memory allocated"));
  }
}

void AsyncServer::setSchema(uint8_t *payloadPtr, uint8_t payloadSize, uint8_t total)
{
	payloadBuf = payloadPtr;
	_payloadSz = payloadSize;
	_payloadTotal = total;
}


void AsyncServer::setJson(toJson_t tojson, uint16_t jsonBufSize)
{
  _toJson = tojson;
  jsonBuffer = (char*)malloc(jsonBufSize);
  if (payloadBuf != NULL)
  {
    Serial.println(F("JSON Memory allocated"));
  }
}

void AsyncServer::setJson(toJson_t tojson, char *buf, uint16_t jsonBufSize = 128)
{
	_toJson = tojson;
	jsonBuffer = buf;
}

void AsyncServer::start()
{
  sendState = READ_MEM;
  bool ok = true;

  if(_toJson)
  {
    if(_send == NULL) ok = false;
  }
  else
  {
    if(_sendL == NULL) ok = false;
  } 

  if(_ackWait == NULL) ok = false;
  // if(_toJson == NULL ) ok = false;

  if(ok == true)
  {
  	Serial.println(F("Server Init ok"));
  }
  else
  {
  	Serial.println(F("Server init failed"));
  }
}

void AsyncServer::printPayload(byte *buf, byte len)
{
  for (byte i = 0; i < len; i++)
  {
    Serial.print(buf[i]); Serial.print(" ");
  }
  Serial.println();
}

void AsyncServer::sendLoop(bool connected)
{
  if (connected)
  {
    // Serial.println(F("Sending Data.."));
    int pipeAck;
    switch (sendState)
    {
      case READ_MEM:
        // Serial.println(F("S_STATE: READ_MEM"));
        this -> payloadPtr = _memQPtr -> read(payloadBuf, _payloadTotal);
        if (payloadPtr != NULL)
        {
          // Serial.println(F("Mem red done----------<"));
          printPayload(payloadPtr,_payloadSz*_payloadTotal);
          if(_toJson)
          {
            // Serial.println(F("Going to json"));
          	sendState = TO_JSON;
          }
          else
          {
            Serial.println(F("Going to SERVER_SEND"));
          	sendState = SERVER_SEND;
          }
          
        }
        break;
      case TO_JSON:
        Serial.println(F("S_STATE: TO_JSON"));
        jsonPtr = _toJson(payloadPtr, jsonBuffer, _payloadTotal);
        if(jsonPtr != NULL )
        {
          Serial.println(jsonPtr);
          sendState = SERVER_SEND;
        }
        else
        {
          //If unsupported packet found. read next memory
          // Serial.println(F("Unsupported Packet"));
          sendState = READ_MEM;
        }
        break;
      case SERVER_SEND:
        Serial.println(F("S_STATE: SEND"));
        if(_toJson)
        {
          // if (jsonPtr != NULL)
          // {
          _send(jsonPtr);
          sendState = WAIT_ACK;
          // }
        }
        else
        {
          Serial.println(F("Sending to Server"));
           // printPayload(payloadPtr,_payloadSz*_payloadTotal);
           _sendL(payloadPtr,_payloadSz*_payloadTotal);
          sendState = WAIT_ACK;
        }
        break;
      case WAIT_ACK:
        Serial.println(F("S_STATE: WAIT"));
        pipeAck = _ackWait();
        Serial.print(F("===> ACK : ")); Serial.println(pipeAck);
        if (pipeAck == 200)
        {
          sendState = SEND_SUCCESS;
        }
        else
        {
          sendState = FAILED;
        }
        break;
      case SEND_SUCCESS:
        Serial.println(F("S_STATE: SUCCESS"));
        sendState = READ_MEM;
        break;
      case FAILED:
        Serial.println(F("S_STATE: FAILED"));
        sendState = SERVER_SEND;
        break;
    }
  }
}
