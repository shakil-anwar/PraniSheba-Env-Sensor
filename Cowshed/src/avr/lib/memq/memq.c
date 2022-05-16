/**********************************************************************
Author : Shuvangkar Chandra Das
Contributor : Shakil Anwar 
Description:  This is generic Ring Queue buffer with locking mechanism. 
The buffer can be used with any type of memory including flash memory, fram, 
ram etc. 
************************************************************************/

#include "memq.h"

#if defined(ARDUINO_ARCH_AVR)  || defined(ARDUINO_ARCH_ESP32)
#if defined(PROD_BUILD)
    #include "../../../common_lib/arduinoCwrapper/Serial.h"
#else
    #include "Serial.h"
#endif
#elif defined(__MSP430G2553__) || defined(__MSP430FR2433__)
    #include <msp430.h>
    #include "mspDriver.h"
#else
    #error "memq did not find chip architecture "
#endif



  uint32_t headCount = 0;
  uint8_t data = 0;
  uint32_t readAddr = 0; 


void eraseMemBlob(struct memq_t *memq);

uint8_t log2base(uint16_t n);
struct memqReadLog_t memqReadLog;



//This function begin memq pointers and initialie valiable from user object structure
void memqBegin(struct memq_t *memq,uint32_t baseAddr, uint32_t packetLen, uint32_t totalPacket)
{
  if (memq != NULL)
  {
    memq->_baseAddr = baseAddr;
    memq->_lastAddr = baseAddr + packetLen * totalPacket;
    memq->_packetLen = packetLen;
    memq->_totalPacket = totalPacket;
    memq->_ptrEventCounter = 0;
    memq->_disableOthers = NULL;
    memq->_enableOthers = NULL;
  }
  

  #if defined(MEMQ_DEBUG)
    SerialPrintF(P("Memq Start : "));
    SerialPrintU32(memq->_baseAddr);  
    SerialPrintF(P(" | End "));
    SerialPrintU32(memq->_lastAddr);
    SerialPrintF(P(" | totPkt "));
    SerialPrintlnU32(memq->_totalPacket);
    
  #endif
}

//This function dynamically allocate memory and then begin memq. 
struct memq_t *memqNew(uint32_t baseAddr, uint32_t packetLen, uint32_t totalPacket)
{
  // SerialBegin(9600);
  struct memq_t *memq = malloc(sizeof(struct memq_t));
  if (memq != NULL)
  {
    memqBegin(memq,baseAddr,packetLen,totalPacket);
  }
  return memq;
}

//This function sets memory function pointers 
void memqSetMem(struct memq_t *memq, memFun_t memReader, memFun_t memWriter, memEraser_t memEraser, uint16_t blobSz)
{
  //SerialPrintlnF(P("setMem Called"));
  memq->_memReader = memReader;
  memq->_memWriter = memWriter;
  memq->_memBlobEraser = memEraser;
  memq->_blobSize = blobSz;
}

//The memory pointer needs to keep track for proper operation. So this function keep track of memory 
//pointer in eeprom. For doing that the corresponding function has to set. 
void memqSetMemPtr(struct memq_t *memq, ringFun_t reader, ringFun_t writer, uint16_t maxPtrEvent)
{
  memq->_ptrRead = reader;
  memq->_ptrWrite = writer;
  memq->_ptrRead(&(memq->ringPtr));
  memq->_ptrRead(&(memq->ringPtr));
  memq->_maxPtrEvent = maxPtrEvent;
  bool notOk = memq->ringPtr._head > memq->_lastAddr ||  
               memq->ringPtr._tail > memq->_lastAddr || 
               memq->ringPtr._saveTail > memq->_lastAddr ||
               memq->ringPtr._head < memq->_baseAddr ||  
               memq->ringPtr._tail < memq->_baseAddr || 
               memq->ringPtr._saveTail < memq->_baseAddr;
  if(notOk)
  {
#if defined(ESP32_V010)
    memqPrintBeginLog(memq);
    memqPrintLog(memq);
#endif
    memqReset(memq);
  }
  // memqReset(memq);
  headCount = 0;
  data = 0;
  readAddr = 0;

  memqLockBus(memq);
//   if(memq->_memReader)
//   {
//     headCount = (uint32_t)0UL;
//     do
//     {
//       readAddr = (uint32_t)memq->ringPtr._head + (uint32_t)headCount * (uint32_t)memq->_packetLen;
      
//       if ((uint32_t)memq->ringPtr._head >= (uint32_t)memq->_lastAddr)
//       {
//         memq->ringPtr._head = (uint32_t)memq->_baseAddr;

//       }
//       memq->_memReader((uint32_t)readAddr,&data,1);
//       headCount++;
//       if((uint32_t)headCount > (uint32_t)memq->_totalPacket)
//       {
// #if defined(ESP32_V010)
//         SerialPrintF(P("Memq Head Count: ")); SerialPrintlnU32((uint32_t)headCount);
//         SerialPrintF(P("Memq total Packet: ")); SerialPrintlnU32((uint32_t)memq->_totalPacket);
//         memqPrintBeginLog(memq);
//         memqPrintLog(memq);
// #endif
//         memqReset(memq);
//         headCount = 0;
//         break;
//       }
//     } while ((data != 255) && (memq->ringPtr.qState != RESET));
//   }
//   memqUnlockBus(memq);

  // // memq->ringPtr._head = memq->ringPtr._head + (headCount-1) *memq->_packetLen;
  //  memq->ringPtr._head = (uint32_t)memq->ringPtr._head + ((uint32_t)headCount) * (uint32_t)memq->_packetLen;
  // SerialPrintF(P("|H:")); SerialPrintlnU32(memq->ringPtr._head);
// #if defined(MEMQ_DEBUG)
//   SerialPrintF(P("memq RingPtr Head : "));
//   SerialPrintU32(memq->ringPtr._head);
//   SerialPrintF(P(" | Tail : "));
//   SerialPrintlnU32(memq->ringPtr._tail);
// #endif



  if ((memq->ringPtr._head == memq->ringPtr.willEraseAddr) && (memq->ringPtr._head != memq->ringPtr._tail))
  {
    memq->ringPtr._isLock = true;
  }
  else
  {
    memq->ringPtr._isLock = false;
  }




  // SerialPrintF(P("head"));SerialPrintU32(memq->ringPtr._head);
  // SerialPrintF(P("|tail"));SerialPrintU32( memq->ringPtr._tail);
  // SerialPrintF(P("|sTail:"));SerialPrintU32( memq->ringPtr._saveTail);
  // SerialPrintF(P("|Ers:"));SerialPrintU32(memq->ringPtr.willEraseAddr);
  // SerialPrintF(P("|qSt:"));SerialPrintU8(memq->ringPtr.qState);
  // SerialPrintF(P("|loc:"));SerialPrintU8(memq->ringPtr._isLock);
  // SerialPrintF(P("|cksm:"));SerialPrintlnU8(memq->ringPtr.checkSum);


  memq->_ptrWrite(&(memq->ringPtr)); //saving pointer in edge conditions
  memqPrintBeginLog(memq);
  memqPrintLog(memq);
#if defined (BOARD_MEGA1284_V010)
  memq->ringPtr._tail = memq->ringPtr._saveTail;
#endif
}


//Print memq  begin log 
void memqPrintBeginLog(struct memq_t *memq)
{
	SerialPrintF(P("MEMQ->BEGIN:"));SerialPrintU32(memq->_baseAddr);
  	SerialPrintF(P("|End:"));SerialPrintU32(memq->_lastAddr);
  	SerialPrintF(P("|H:")); SerialPrintU32(memq->ringPtr._head);
    SerialPrintF(P("|T:")); SerialPrintU32(memq->ringPtr._tail);
    SerialPrintF(P("|A:")); SerialPrintlnU32(memq->ringPtr._saveTail);
}
//Where there are multiple spi devices in memory spi bus, The bus needs to lock before flash read or write operation, 
//So this function attach two functions which lock and unlock the SPI bus for memory operation. 
void memqAttachBusSafety(struct memq_t *memq, void (*enableOthers)(void), void (*disableOthers)(void))
{
  memq->_enableOthers = enableOthers;
  memq->_disableOthers = disableOthers;
}

//log2base faster implementation 
uint8_t log2base(uint16_t n)
{
    uint8_t inc=0;
    while(n)
    {
        n = n>>1;
        inc++;
    }
    inc--;
    return inc;
}

//lock spi bus
void memqLockBus(struct memq_t *memq)
{
  if (memq->_disableOthers)
  {
    memq->_disableOthers();
  }
}

//unlock spi bus 
void memqUnlockBus(struct memq_t *memq)
{
  if(memq->_enableOthers)
  {
    memq->_enableOthers();
  }
}

//reset memq memory. So this function erase full memoery and reset pointer
void memqReset(struct memq_t *memq)
{
   uint8_t blobIndex = log2base(memq->_blobSize);
  uint32_t startBlob = (memq->_baseAddr) >> (blobIndex);
  uint32_t endBlob = (memq->_lastAddr - 1) >> (blobIndex);
#if defined(MEMQ_DEBUG)
  SerialPrintlnF(P("Resetting MemQ"));
  SerialPrintF(P("StartBlob : "));
  SerialPrintlnU32(startBlob);
  SerialPrintF(P("EndBlob : "));
  SerialPrintlnU32(endBlob);
  SerialPrintF(P("Blob Sz : "));
  SerialPrintlnU16(memq->_blobSize);
  SerialPrintF(P("Blob Index :"));
  SerialPrintlnU8(blobIndex);
#endif
  uint32_t blobAddr;
  uint32_t i;
  for (i = startBlob; i <= endBlob; i++)
  {
    blobAddr = i << (blobIndex);
    SerialPrintF(P("Erasing Blob : "));
    SerialPrintlnU32(i);
    memq->_memBlobEraser(blobAddr, memq->_blobSize);
  }
  //erase ringeeprom
  memq->ringPtr._head = memq->_baseAddr;
  memq->ringPtr._tail = memq->_baseAddr;
  memq->ringPtr._saveTail = memq->_baseAddr;
  memq->ringPtr.willEraseAddr = memq->_baseAddr;
  memq->ringPtr.qState = RESET;
  memq->ringPtr._isLock = false;
  memq->ringPtr.checkSum = 0;
  memq->_ptrWrite(&(memq->ringPtr));
  //reset variables
  memq->_ptrEventCounter = 0;

}

//write a full  packet  into memory. So it will take only data pointer. 
void memqWrite(struct memq_t *memq, uint8_t *buf)
{
  if (memq->ringPtr._isLock == false)
  {
    // memqLockBus(memq);
    memq->_memWriter(memq->ringPtr._head, buf, memq->_packetLen);
    // memqUnlockBus(memq);

    memq->ringPtr._head += memq->_packetLen;
    memq->ringPtr.qState = RUNNING;
    memq->_ptrEventCounter++;

    //Reset buffer logic
    if (memq->ringPtr._head >= memq->_lastAddr)
    {
      memq->ringPtr._head = memq->_baseAddr;
      memq->_ptrWrite(&(memq->ringPtr)); //saving pointer in edge conditions
    }
    //determine lock condition
    if (memq->ringPtr._head == memq->ringPtr.willEraseAddr)
    {
      memq->ringPtr._isLock = true;
      memq->_ptrWrite(&(memq->ringPtr)); //saving pointer in edge conditions
      // memqLockBus(memq);
// #if defined(MEMQ_DEBUG)
//       SerialPrintlnF(P("memq lock condition"));
// #endif
    }
  }
  // else
  // {
  //   SerialPrintlnF(P("<memq locked>"));
  // }
}


//Print log information while reading 
void memqPrintReadLog(struct memq_t *memq,struct memqReadLog_t *log)
{
  SerialPrintF(P("MEMQ->READ->dAvail:"));SerialPrintU8(log->isDataAvailable);
  SerialPrintF(P("|BlbErs:"));SerialPrintU8(log->isBlobErased);
  SerialPrintF(P("|lstBlbErs:"));SerialPrintU8(log->isLastBlobErased);
  SerialPrintF(P("|isRst:"));SerialPrintlnU8(memq->ringPtr.qState == RESET);
}


void eraseMemBlob(struct memq_t *memq)
{
  // memqLockBus(memq);
  memq->_memBlobEraser(memq->ringPtr.willEraseAddr, memq->_blobSize);
  // memqUnlockBus(memq);
  memq->ringPtr._isLock = false; //lock open after each blob erase
  memq->ringPtr.willEraseAddr += memq->_blobSize;
  memq->_ptrWrite(&(memq->ringPtr)); //saving pointer in edge condition

  memqReadLog.isBlobErased = true;
  memqReadLog.isLastBlobErased = false;      
}


//Read a full packet from memory, So it will take a buffer pointer where data will be written 
uint8_t *memqRead(struct memq_t *memq, uint8_t *buf)
{
  
  if (memq->ringPtr.qState != RESET)
  {
    if (memq->ringPtr.qState != NO_DATA)
    {

      // memqLockBus(memq);
      memq->_memReader(memq->ringPtr._tail, buf, memq->_packetLen); //read from flash
      // memqUnlockBus(memq);



      memq->ringPtr._tail += memq->_packetLen;
      memq->_ptrEventCounter++;
#if defined (BOARD_MEGA1284_V010)
  //pointer reset logic
      if (memq->ringPtr._tail == memq->_lastAddr)
      {
        memq->ringPtr._tail = memq->_baseAddr;
      }
      if (memq->ringPtr._saveTail >= (memq->ringPtr.willEraseAddr + memq->_blobSize))
      {
        if(memq->ringPtr._tail >= (memq->ringPtr.willEraseAddr + memq->_blobSize))
        {
          eraseMemBlob(memq);
        }else{
          memqReadLog.isBlobErased = false;
          memqReadLog.isLastBlobErased = false;
        }
      }
      else if(memq->ringPtr._saveTail < memq->ringPtr.willEraseAddr)
      {
          if(memq->ringPtr._tail < (memq->ringPtr.willEraseAddr))
          {
            eraseMemBlob(memq);
            if(memq->ringPtr.willEraseAddr >= memq->_lastAddr)
            {
              memq->ringPtr.willEraseAddr = memq->_baseAddr;
            }
          }
          
      }

#else
      //pointer reset logic
      if (memq->ringPtr._tail == memq->_lastAddr)
      {
        memq->ringPtr._tail = memq->_baseAddr;
        //erase last blob and reset blob address
        // #if defined(MEMQ_DEBUG)
        // SerialPrintF(P("Erasing Last Addr :"));
        // SerialPrintlnU32(memq->ringPtr.willEraseAddr);
        // #endif
        // memqLockBus(memq);
        memq->_memBlobEraser(memq->ringPtr.willEraseAddr, memq->_blobSize);
        // memqUnlockBus(memq);
        memq->ringPtr._isLock = false; //lock open after each blob erase
        memq->ringPtr.willEraseAddr = memq->_baseAddr;
        memq->_ptrWrite(&(memq->ringPtr)); //saving pointer in edge condition

        memqReadLog.isLastBlobErased = true;
        memqReadLog.isBlobErased = false;
      }
      else if (memq->ringPtr._tail >= (memq->ringPtr.willEraseAddr + memq->_blobSize)) //4 is blob size
      {
        eraseMemBlob(memq);
      }
      else
      {
        memqReadLog.isBlobErased = false;
        memqReadLog.isLastBlobErased = false;
        // SerialPrintlnF(P("No action on memory"));
      }
#endif

      memqReadLog.isDataAvailable = true;
      //check data availability
      if (memq->ringPtr._tail == memq->ringPtr._head)
      {
        // #if defined(MEMQ_DEBUG)
        // SerialPrintlnF(P("Data Finished"));
        // #endif
        memq->ringPtr.qState = NO_DATA;
        memqReadLog.isDataAvailable = false;
        memq->ringPtr._isLock = false;
      }

      memqPrintReadLog(memq, &memqReadLog);
      return buf;
    }
    else
    {
      //SerialPrintlnF(P("qState : NO_DATA"));
      return NULL;
    }
  }
  else
  {
    //SerialPrintlnF(P("RESET: Data NULL"));
    return NULL;
  }
}

//This functions memory pointer data into flash memory or user defined memory space
void memqSaveMemPtr(struct memq_t *memq)
{
  if (memq->_ptrEventCounter >= memq->_maxPtrEvent)
  {
#if defined(MEMQ_DEBUG)
    SerialPrintlnF(P("Saving Pointer.."));
#endif
    memq->_ptrWrite(&(memq->ringPtr));
    memq->_ptrEventCounter = 0;
  }
}

//return number of available packet in the memory 
uint32_t memqAvailable(struct memq_t *memq)
{
   if (memq->ringPtr._tail == memq->ringPtr._head)
  {
     return 0;  
  }
  else if(memq->ringPtr._head > memq->ringPtr._tail)
  {
    return ((memq->ringPtr._head - memq->ringPtr._tail)/memq->_packetLen);
  }
  else
  {
    uint32_t len = (memq->_lastAddr - memq->ringPtr._tail) + (memq->ringPtr._head - memq->_baseAddr);
    len = len/memq->_packetLen;
    return  len;
  }
}

//return memq lock status 
bool memqIsLock(struct memq_t *memq)
{
  return memq->ringPtr._isLock;
}

//print memq basic log for debugging
void memqPrintLog(struct memq_t *memq)
{
  static uint16_t prevCounter;
  if(memq->_ptrEventCounter != prevCounter)
  {
    SerialPrintF(P("MEMQ->"));
    SerialPrintF(P("H:")); SerialPrintU32(memq->ringPtr._head);
    SerialPrintF(P("|T:")); SerialPrintU32(memq->ringPtr._tail);
    SerialPrintF(P("|nxtErs:")); SerialPrintU32(memq->ringPtr.willEraseAddr);
    SerialPrintF(P("|isLk:"));SerialPrintU8(memq->ringPtr._isLock);
    SerialPrintF(P("|ptrSaved:"));SerialPrintlnU8(memq->_ptrEventCounter==0);
    prevCounter = memq->_ptrEventCounter;
  }
}



