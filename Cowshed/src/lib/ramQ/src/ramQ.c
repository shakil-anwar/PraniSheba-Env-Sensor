#include "ramQ.h"

#if defined(PROD_BUILD)
    #include "../../arduinoCwrapper/Serial.h"
#else
    #include "Serial.h"
#endif

void *_baseAddr;
void *_lastAddr;
void *_middleAddr;

void *_ramQHead;
void *_ramQTail;
void *_ramQFlash; //This pointers point base when ramq is half full & points milld when ramq is full
void *_ramQBase;  //this pointer points base address when ramQ is full

int8_t ramQCounter = 0;
int8_t ramQTailCounter = 0;
uint8_t _packetSz;


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
  SerialBegin(250000);

}

void *ramQHead()
{
//  Serial.print(F("Count :")); Serial.println(ramQCounter);
  
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

void ramQwrite(void *bufPtr)
{
  memcpy( _ramQHead, bufPtr, _packetSz);
}
