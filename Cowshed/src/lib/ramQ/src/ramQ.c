#include "ramQ.h"

#if defined(ARDUINO_ARCH_AVR)
    #include <Arduino.h>
    #if defined(PROD_BUILD)
        #include "../../arduinoCwrapper/Serial.h"
    #else
        #include "Serial.h"
    #endif
#elif defined(ARDUINO_ARCH_SAM)
    #include <Arduino.h>
#elif defined(__MSP430G2553__) || defined(__MSP430FR2433__)
    #include <msp430.h>
    #include "mspDriver.h"
#else
    #error "ramq did not find chip architecture "
#endif

enum ramqState_t
{
	AVAIL_DATA,
	NO_DATA,
	RESET,
};



volatile void *_baseAddr; //points to the base address
volatile void *_lastAddr;
volatile void *_middleAddr; //points to the middle  address

volatile void *_ramQHead; //head pointer
volatile void *_ramQTail; //tail pointer


volatile void *_ramQFlash; //This pointers point base when ramq is half full & points milld when ramq is full
volatile void *_ramQBase;  //this pointer points base address when ramQ is full

volatile int8_t ramQCounter = 0;
volatile int8_t ramQTailCounter = 0;
volatile uint8_t _packetSz;

volatile bool _isRamQLock;

enum ramqState_t ramqState;

void ramQSet(void *bufPtr, uint8_t packetSz, uint8_t totalPacket)
{
  _baseAddr = bufPtr;
  _lastAddr = _baseAddr + packetSz * totalPacket;

  _ramQHead = _baseAddr;
  _ramQTail = _baseAddr;

  _packetSz = packetSz;

  _ramQFlash = NULL;
  _ramQBase  = NULL;
  //  _middleAddr = _baseAddr + (totalPacket >> 2) * packetSz;
  _middleAddr = _baseAddr + (((uint8_t*)_lastAddr - (uint8_t*)_baseAddr) / 2);
  // SerialBegin(250000);

  _isRamQLock = false;
  ramqState = RESET;

}

void *ramqGetHead()
{
	void *tempHead = NULL;
	if(_isRamQLock == false)
	{
		tempHead = _ramQHead;
		_ramQHead += _packetSz;
		ramQCounter++;
		//determine reset condition
		if(_ramQHead == _lastAddr)
		{
			_ramQHead = _baseAddr;
			ramQCounter = 0;
			// SerialPrintlnF(P("Head Reset"));
		}

		//determine lock condition
		if(_ramQHead == _ramQTail)
		{
		  	_isRamQLock = true;
		  	// SerialPrintlnF(P("ramq locked"));
		}

		ramqState = AVAIL_DATA;
	}

	// SerialPrintF(P("ramq counter:"));SerialPrintlnU8(ramQCounter);
	return tempHead;
}

bool ramqIsLocked()
{
	return _isRamQLock;
}

uint16_t ramqAvailable()
{
	uint16_t len;
	if(_ramQHead == _ramQTail)
	{
		if(ramqState == AVAIL_DATA)
		{
			len = _lastAddr - _baseAddr;
		}
		else
		{
			len = 0;
		}
	}
	else if(_ramQHead>_ramQTail)
	{
		len = _ramQHead - _ramQTail;
	}
	else
	{
		len = (_lastAddr - _ramQTail) + (_ramQHead - _baseAddr);
	}
	len = len/_packetSz;
	return len;
}

void *ramQHead()
{
 // Serial.print(F("Count :")); Serial.println(ramQCounter);
  
  return _ramQHead;
}

void *ramQUpdateHead()
{
  ramQCounter ++;
  _ramQHead += _packetSz;
  if (_ramQHead >= _middleAddr)
  {
    if (_ramQHead == _middleAddr)
    {
//      Serial.println(F("Flash got address"));
      _ramQFlash = _baseAddr;
    }
    else if (_ramQHead == _lastAddr)
    {
      ramQCounter = 0;
//      Serial.println(F("Head Reset"));
      _ramQHead = _baseAddr;
      _ramQFlash = _middleAddr;
      _ramQBase = _ramQHead; 
    }
  }

  return _ramQHead;
}

void *ramqGetTail()
{
	void *tempTail = NULL;
	if(ramqState == AVAIL_DATA)
	{
		tempTail = _ramQTail;	
		
    	_ramQTail  += _packetSz;
    	ramQTailCounter++;

		//reset logic
		if (_ramQTail == _lastAddr)
		{
		    _ramQTail = _baseAddr;
		    // SerialPrintlnF(P("Tail Reset"));
		    ramQTailCounter = 0;
		}

		//check data availability
		if(_ramQTail == _ramQHead)
		{
			ramqState = NO_DATA;
			// tempTail = NULL;
		}

		_isRamQLock = false;
	}
	// SerialPrintF(P("Tail Addr:"));
	// SerialPrintU16((uint16_t)tempTail);
	// SerialPrintF(P("|Counter:"));
	// SerialPrintlnU8(ramQTailCounter);
	return tempTail;
}




void *ramQRead()
{
  if(_ramQTail<_ramQHead)
  {
    return _ramQTail;
  }
  else if(_ramQTail > _ramQHead)
  {
    return _ramQTail;
  }
  else if(_ramQTail == _ramQHead)
  {
    // SerialPrintlnF(P("RamQ Tail Null"));
    return NULL;
  }
  return NULL;



//   if (_ramQTail == _ramQHead)
//   {
//     SerialPrintlnF(P("RamQ Tail Null"));
//     return NULL;
//   }
//   else
//   {
// //    Serial.println(F("Read NOT NULL"));
//     return _ramQTail;
//   }
}

void *ramQUpdateTail()
{
  if(_ramQTail != _ramQHead)
  {
    ramQTailCounter++;
    _ramQTail  += _packetSz;
  }
  
  
  if (_ramQTail == _lastAddr)
  {
    _ramQTail = _baseAddr;
    ramQTailCounter = 0;
  }
  // return ramQRead();
}


void ramqPrintLog()
{
	static void *prevHead = NULL;
	if(_ramQHead != prevHead)
	{
		SerialPrintF(P("RAMQ->Head:"));SerialPrintU16((uint16_t)_ramQHead);
		SerialPrintF(P("|isHeadRst:"));SerialPrintU8(_ramQHead == _baseAddr);
		SerialPrintF(P("|isLock:"));SerialPrintU8(_isRamQLock);
		SerialPrintF(P("|headCount:"));SerialPrintlnU8(ramQCounter);
		prevHead = _ramQHead;
	}
	
	static void *prevTail = NULL;
	if(_ramQTail != prevTail)
	{
		SerialPrintF(P("RAMQ->Tail:"));SerialPrintU16((uint16_t)_ramQTail);
		SerialPrintF(P("|isTailRst:"));SerialPrintU8(_ramQTail == _baseAddr);
		SerialPrintF(P("|isDataAvail:"));SerialPrintU8(ramqState == AVAIL_DATA);
		SerialPrintF(P("|TailCount:"));SerialPrintlnU8(ramQTailCounter);
		prevTail = _ramQTail;
	}
}


