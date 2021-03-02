#ifndef _RAMQ_H_
#define _RAMQ_H_
//#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


void ramQSet(void *bufPtr, uint8_t packetSz, uint8_t totalPacket); //ramQset(&payload[0],sizeof(payload_t),TOTAL_PAYLOAD)

void *ramQHead();
void *ramQUpdateHead();// increment current head pointer and return head pointer.

void *ramQRead();
void *ramQUpdateTail();


void ramQwrite(void *bufPtr);


extern void *_ramQHead;
extern void *_ramQTail;
extern void *_ramQFlash;
extern void *_ramQBase;

extern int8_t ramQCounter;
extern int8_t ramQTailCounter;
#ifdef __cplusplus
}
#endif

#endif
