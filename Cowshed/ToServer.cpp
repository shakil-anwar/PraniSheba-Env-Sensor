#include "ToServer.h"
#include "dataSchema.h"
#include "pin.h"
#include "radio.h"

#define PAYLOAD_IN_A_PACKET         1
#define TOTAL_BYTES_IN_JSON         (PAYLOAD_IN_A_PACKET*MAX_PAYLOAD_BYTES)
#define TOTAL_JSON_PACKET_IN_PAGE   (FLASH_MEM_PAGE_BYTES/TOTAL_BYTES_IN_JSON)
#define DATA_SEND_OP_CODE           10
#define MAX_RETRY                   15

payload_t payload1;

uint8_t *flashPagePtr;
uint8_t *payloadPtr;

char pipeBuf[256];


uint8_t packetCounter;
char *json;
int radioAck;


dataXferState_t dataXferState;
pipestate_t radioState;
pipestate_t checkRadio;


uint32_t currentSendMillis;
uint32_t prevSendMillis;

void toSetverBegin()
{
  dataXferState = READ_FLASH;
  radioState = DATA_SENT;
  packetCounter = 0;
  retryCount = 0;
  Serial.println(F("Server begin done"));
}
void dataSendStateMachine()
{
  if (1)
  {
    switch (dataXferState)
    {
      case READ_FLASH:
//        Serial.println(F("S_STATE: READ_FLASH"));
//        void *buf = (void*)&payload1;
        flashPagePtr = (uint8_t*)memQ.read(&payload1);
        if (flashPagePtr != NULL)
        {
          payloadPtr = flashPagePtr;
          packetCounter = 0;
          dataXferState = SEND;
        }
        break;
//      case TO_JSON:
//        Serial.println(F("S_STATE: TO_JSON"));
//        json = toJson(pipeBuf, payloadPtr, PAYLOAD_IN_A_PACKET);
//        //          json = toJson((payload_t *)payloadPtr, pipeBuf);
//        Serial.println(json);
//        dataXferState = SEND;
//        break;
      case SEND:
        Serial.println(F("S_STATE: SEND"));

          nrf_send(payloadPtr);
          dataXferState = WAIT;

        break;
      case WAIT:
        Serial.println(F("S_STATE: WAIT"));
        if (nrf_send_success == true)
        {
          nrf_send_success = false;
          packetCounter++;
          dataXferState = SUCCESSFUL;
          prevSendMillis = millis();
        }
        else
        {
//          retryCount++;
          if (retryCount < MAX_RETRY)
          {
            dataXferState = SEND;
          }
          else
          {
            dataXferState = NO_CONNECTION;
          }
        }
        break;
      case SUCCESSFUL:
        Serial.println(F("S_STATE: SUCCESS"));
        if (packetCounter < 1)
        {
          json = NULL;
          payloadPtr += TOTAL_BYTES_IN_JSON;
          dataXferState = SEND;
        }
        else
        {
          dataXferState = READ_FLASH;
          payloadPtr = NULL;
          flashPagePtr = NULL;
        }
        break;
      case NO_CONNECTION:
        Serial.println(F("S_STATE: NO_CONNECTION"));
        if (1)
        {
          dataXferState = SEND;
        }
        break;
    }
  }

}

//pipestate_t pipeAckSend(uint8_t opCode, const char *data)
//{
//  switch (pipeState)
//  {
//    case DATA_SENT:
//      Serial.println(F("P_STATE: DATA_SENT"));
//      pipe.send(opCode, data);
//      pipeState = WAIT_ACK;
//      break;
//    case WAIT_ACK:
//      Serial.println(F("P_STATE: WAIT_ACK"));
//      pipeAck = pipe.waitForAck();
//      if (pipeAck == 200)
//      {
//        pipeState = SENT_SUCCESS;
//      }
//      break;
//    case SENT_SUCCESS:
//      Serial.println(F("P_STATE: SENT_SUCCESS"));
//      break;
//  }
//  return pipeState;
//}
