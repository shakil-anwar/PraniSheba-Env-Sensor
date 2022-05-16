
#ifndef _RAMQ_PRO_H_
#define _RAMQ_PRO_H_

#include <Arduino.h>
// #include "Serial.h"

#ifdef __cplusplus
extern "C" {
#endif


#define SEND_TO_MQTT			1
#define MQTT_SUCCESSFUL		2
#define DATA_RECEIVED			3

//|....qObj_t.....|.........Data.............|

struct qObj_t
{
	uint8_t *ptr; //point current data pointer 
	uint8_t *nextPtr; //keep track of the next buffer pointer 
	uint8_t mgsId;
	uint8_t mgsState;
	uint16_t len;  //data length of current pointer
}__attribute__((packed));

enum ringqState_t
{
  RINGQ_RESET = 0,
  RINGQ_RUNNING = 1,
  RINGQ_NO_DATA = 3,
};

struct ringq_t
{
	bool _isLock;
	bool _leadingHead; //true means head > tail | false means head < tail
	uint8_t *_baseAddr;
	uint8_t *_endAddr;
	uint32_t _len;
	struct qObj_t _head;
	struct qObj_t _tail;
	enum ringqState_t _qState;
}__attribute__((packed));

struct ringq_t *ringqNew(void *baseAddr, uint32_t len);
bool ringqPush(struct ringq_t *me, void *dataPtr, uint16_t len);
bool ringqPushMid(struct ringq_t *me, void *dataPtr, uint8_t mgsId, uint16_t len);
struct qObj_t *ringqNextTail(struct ringq_t *me);
struct qObj_t *ringqPop(struct ringq_t *me);
void ringqReset(struct ringq_t *ramq);
bool ringqIsLocked(struct ringq_t *ramq);
uint32_t ringqNextPacketLen(struct ringq_t *ramq);



extern struct ringq_t *ringqObj;

#ifdef __cplusplus
}
#endif


#endif