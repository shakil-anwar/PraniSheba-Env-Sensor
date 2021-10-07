#ifndef _NRF24_QUERY_COMMON_H_
#define _NRF24_QUERY_COMMON_H_

#ifdef __cplusplus
extern "C"
{
#endif


#if defined(ARDUINO_ARCH_AVR)
    #include <Arduino.h>
    #if defined(PROD_BUILD)
        #include "../Sensor_TDM/TDM.h"
    #else
        #include "TDM.h"
    #endif
#elif defined(__MSP430G2553__) || defined(__MSP430FR2433__)
      #include "mspIoT.h"
#else
    #error "nRF24_DRIVER did not find chip architecture "
#endif

#include "nRF24.h"

#define QUERY_TIMEOUT_MS 1000
#define PING_TYPE 200
#define PING_OPCODE 201     // register opcode
#define PING2_OPCODE 203    // data send ping opcode
#define ADDR_OPCODE 221
#define NRF_CONFIG_OPCODE 225


 typedef struct pipeAddr_t
{
    uint8_t pipe0Addr[5];
    uint8_t pipe1Addr[5];
    uint8_t pipe2Addr;
    uint8_t pipe3Addr;
    uint8_t pipe4Addr;
    uint8_t pipe5Addr;
}pipeAddr_t;


typedef struct query_t
{
    uint16_t bsId;
    uint16_t deviceId;
    uint8_t slotId;
    uint8_t flag;
    uint8_t type;
    uint8_t opcode;
    uint8_t reserve;
    uint8_t checksum;
} query_t;

typedef struct pong_t
{
    uint32_t second;
    uint32_t ms;
    uint16_t bsId;
    uint16_t deviceId;

    uint8_t type;
    uint8_t flag;
    uint8_t opcode;

    uint8_t isConfigChanged;
    bool isBsFree;
    bool isMySlot;
    
    uint8_t checksum;
} pong_t;


typedef struct nrfNodeConfig_t
{
    uint16_t bsId;
    uint16_t deviceId;
    uint16_t dataFreq;      // sampling frequncy
    uint16_t momentDuration;        // Time period
    uint16_t perNodeInterval;       // per slot time

    uint8_t opcode;
    uint8_t slotId;
    uint8_t flag;
    uint8_t type;
    uint8_t node[5];
    uint8_t dataPipeLsbByte;
    uint8_t xPipe[3];
    uint8_t checksum;
} nrfNodeConfig_t;

struct qryObj_t
{
    //common param
    uint16_t bsId;
    uint8_t pipe;  //query pipe no
    uint8_t *activePingAddr; //query common addr
    //Base station param
    uint8_t *bufPtr; //query buffer
    uint8_t len;     //query buffer length
    pipeAddr_t *pipeAddr;
    uint8_t *(*resolver)(uint8_t *);
};

enum addrMode_t
{
    COMMON_PING,
    BS_PING,
    BS_DATA,
    BROADCAST
};


void nrfTxReady(nrfNodeConfig_t *conPtr);
void printPing( pong_t *pingPtr);
void nrfPrintConfig(nrfNodeConfig_t *nrfConf);


extern volatile struct qryObj_t nrfQryObj;


#ifdef __cplusplus
}
#endif

#endif

