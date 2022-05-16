#include "ringQ.h"




struct  ringq_t *ringqNew(void *baseAddr, uint32_t len)
{
	// SerialPrintln("Allocating Space");
	struct ringq_t *me = malloc(sizeof(struct ringq_t));
	// SerialPrintln("Allocation done");
	if(me != NULL)
	{
		me -> _baseAddr = baseAddr;
		me -> _len = len;
		me -> _endAddr = (uint8_t*)baseAddr + len;

		/********Serial only for arduino ***********/
		// SerialPrint("[RingQ] Starting address: ");
		// SerialPrintlnU32(me -> _baseAddr);
		// SerialPrint("[RingQ] Ending address: ");
		// SerialPrintlnU32(me -> _endAddr);

		ringqReset(me);
	}
	return me;
}

bool  ringqPush(struct ringq_t *me, void *dataPtr, uint16_t len)
{
	if(me -> _isLock == false)
	{
		len += sizeof(struct qObj_t);
		uint8_t *nextHead;

		
		nextHead = me -> _head.nextPtr + len; //update data end

		//reset logic for pointer
		if(nextHead > me -> _endAddr)
		{
			// me -> _head.nextPtr = me -> _baseAddr;
			// me -> _head.nextPtr =  nextHead;
			nextHead = me -> _baseAddr;
			if(nextHead <= me -> _tail.nextPtr)
			{
				me -> _leadingHead = false;
			}
			
		}
		

		//determine lock conditions
		if(me -> _leadingHead == false)
		{
			if(nextHead >= me -> _tail.nextPtr)
			{
				//need to reset of cannot save data
				me -> _isLock = true;
				// SerialPrintln("[RingQ_PUSH] is Lockedd");
				return false;
			}else if((nextHead == me -> _baseAddr) && ((nextHead+len) >= me -> _tail.nextPtr))
			{
				me -> _isLock = true;
				// SerialPrintln("[RingQ_PUSH] is Lockeddd");
				return false;
			}

			if(nextHead == me -> _baseAddr)
			{
				bool lastBuffPtr = (me -> _tail.nextPtr == me -> _head.nextPtr);
				me -> _head.nextPtr =  nextHead;
				memcpy(me -> _head.ptr, &(me ->_head),sizeof(struct qObj_t));

				if(lastBuffPtr)
				{
					memcpy(&(me -> _tail), me -> _tail.ptr, sizeof(struct qObj_t));
					me -> _leadingHead = true;	
				}				
			}			
		}

		me -> _head.ptr = me -> _head.nextPtr;
		

		// //initiate first tail nextptr
		// if(me -> _head.ptr == _baseAddr)
		// {
		// 	me -> _tail.nextPtr = nextHead;
		// }

		me -> _head.nextPtr = me -> _head.ptr + len;

		me -> _head.len = len - sizeof(struct qObj_t);
		// me -> _head.nextPtr = nextHead;

		

		memcpy(me -> _head.ptr, &(me ->_head),sizeof(struct qObj_t));
		
		uint8_t *dataPtrHead = me -> _head.ptr + sizeof(struct qObj_t);

		// SerialPrint("[RingQ_PUSH] Write address: ");
		// SerialPrintU32(me -> _head.ptr);
		// SerialPrint(" Data address: ");
		// SerialPrintU32(dataPtrHead);
		// SerialPrint(" Next Head address: ");
		// SerialPrintlnU32(me -> _head.nextPtr);

		memcpy(dataPtrHead,dataPtr,me -> _head.len);

		//increment to next head
		// me -> _head.ptr = nextHead;
		// if( me -> _tail.ptr == me -> _tail.nextPtr)
		// {
		// 	memcpy(&(me -> _tail) , &(me -> _head), sizeof(struct qObj_t));
		// }

		me -> _qState  = RINGQ_RUNNING;
		return true;
	}else
	{
		// SerialPrintln("[RingQ_PUSH] is Locked");
	}
	return false;
	
}

bool ringqPushMid(struct ringq_t *me, void *dataPtr, uint8_t mgsId, uint16_t len)
{
	if(ringqPush(me,dataPtr,len))
	{
		me -> _head.mgsId = mgsId;
		me -> _head.mgsState = DATA_RECEIVED;
		return true;
	}
	else
	{
		return false;
	}
	
}

struct qObj_t *ringqNextTail(struct ringq_t *me)
{
	if(me -> _qState == RINGQ_RUNNING)
	{
		struct qObj_t currentTail;
		memcpy((uint8_t *)&currentTail, me -> _tail.nextPtr, sizeof(struct qObj_t));
		return (struct qObj_t *)(currentTail.ptr);
	}
	// else if(me -> _qState == NO_DATA)
	// {
	// 	// SerialPrintln("[RingQ_POP] NO_DATA available to read.");
	// }
	// else if(me -> _qState == RESET)
	// {
	// 	// SerialPrintln("[RingQ_POP] RESET conditions.");
	// }

	return 0;
}



struct qObj_t *ringqPop(struct ringq_t *me)
{
	if(me -> _qState == RINGQ_RUNNING)
	{

		memcpy(&(me -> _tail), me -> _tail.nextPtr, sizeof(struct qObj_t));	
		struct qObj_t *currentTailObj= (struct qObj_t *)(me -> _tail.ptr);
		uint8_t *nextTailPtr = me -> _tail.nextPtr;

		// SerialPrint("[RingQ_POP] Read address: ");
		// SerialPrintU32(me -> _tail.ptr);
		// SerialPrint(" Next Tail address: ");
		// SerialPrintlnU32(me -> _tail.nextPtr);

		// memcpy(&(me -> _tail), me -> _tail.nextPtr, sizeof(struct qObj_t));	

		

		

		//reset logic for pointer. next tail will be
		// automically point in posh operation

		if(nextTailPtr != me -> _head.nextPtr)
		{
			
			if(nextTailPtr < me -> _head.nextPtr)
			{
				me ->_leadingHead = true;	
			}
			
			
		}else{
			// me -> _isLock = false;
			me -> _qState = RINGQ_NO_DATA;
			me -> _leadingHead = true;
		}
		me -> _isLock = false;

		//check data availability
		// if(_leadingHead)
		// {
		// 	if(nextTail >= me -> _head.ptr)
		// 	{
		// 		me -> _qState = NO_DATA;
		// 	}
		// }

		return currentTailObj;
	}
	else if(me -> _qState == RINGQ_NO_DATA)
	{
		// SerialPrintln("[RingQ_POP] NO_DATA available to read.");
	}
	else if(me -> _qState == RINGQ_RESET)
	{
		// SerialPrintln("[RingQ_POP] RESET conditions.");
	}

	return 0;
}

uint32_t ringqNextPacketLen(struct ringq_t *ramq)
{
	struct qObj_t *currentTailObj = (struct qObj_t *)(ramq -> _tail.nextPtr);
	return currentTailObj->len;
}



void ringqReset(struct ringq_t *me)
{
	memset(me -> _baseAddr, '\0',me -> _len);
	me -> _head.ptr = me -> _baseAddr;
	me -> _head.nextPtr = me -> _baseAddr;
	me -> _head.len = NULL;

	me -> _tail.ptr = me -> _baseAddr;
	me -> _tail.nextPtr = me -> _baseAddr;
	me -> _tail.len = NULL;


	me -> _isLock = false;
	me -> _leadingHead = true;
	me -> _qState = RINGQ_RESET;
}

bool ringqIsLocked(struct ringq_t *ramq)
{
	return ramq ->_isLock;
}




