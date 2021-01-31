#ifndef _ASYNC_SERVER_H
#define _ASYNC_SERVER_H
#include <Arduino.h>
#include "MemQ.h"

typedef void (*send_t)(const char*);
typedef void (*sendL_t)(const uint8_t*,uint8_t);
typedef int (*ackWait_t)(void);
typedef char *(*toJson_t)(uint8_t*, char*, uint8_t); //payload, json buffer, total

typedef enum state_e
{
  READ_MEM,
  TO_JSON,
  SERVER_SEND,
  WAIT_ACK,
  SEND_SUCCESS,
  FAILED,
};



class AsyncServer
{
  public:
  	AsyncServer(MemQ *memQPtr);
    void setServerCbs(send_t send, ackWait_t ackFunc);
    void setServerCbs(sendL_t send, ackWait_t ackFunc);
    void setSchema(uint8_t payloadSize, uint8_t total = 1);
    void setSchema(uint8_t *payloadPtr, uint8_t payloadSize, uint8_t total);
    void setJson(toJson_t tojson, uint16_t jsonBufSize = 128);
    void setJson(toJson_t tojson, char *buf, uint16_t jsonBufSize = 128);
    void start();
    void sendLoop(bool connected);
    void printPayload(byte *buf, byte len);
  private:
    MemQ *_memQPtr;
    state_e sendState;

    send_t  _send = NULL;
    sendL_t _sendL =NULL ;
    ackWait_t _ackWait = NULL;
    toJson_t _toJson = NULL;

    char *payloadBuf = NULL;
    char *jsonBuffer = NULL;
    uint8_t _payloadTotal; //total number of payload packet
    uint8_t _payloadSz;   //a single payload size

    char  *payloadPtr = NULL;
    char  *jsonPtr   = NULL;
};
#endif
