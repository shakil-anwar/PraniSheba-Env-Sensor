#ifndef _TO_SERVER_H_
#define _TO_SERVER_H_
#include <Arduino.h>
#include "all_global.h"
//#include "all_global.h"

typedef enum dataXferState_t
{
  READ_FLASH,
  TO_JSON,
  SEND,
  WAIT,
  SUCCESSFUL,
  NO_CONNECTION
};

typedef enum pipestate_t 
{
  WAIT_FOR_DATA,
  DATA_SENT,
  WAIT_ACK,
  SENT_SUCCESS
};



void toSetverBegin();
pipestate_t pipeAckSend(uint8_t opCode, const char *data);
bool dataSendToServer(uint8_t opcode,const char *data);


#endif
