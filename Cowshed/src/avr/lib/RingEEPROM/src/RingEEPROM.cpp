/************************************************************************
* The idea of the library is simple. I am  Saving a particular parameters
* into multiple location of the eeprom. Suppose I want to store a variable
* in eeprom. Everytime I want to store the variable, I change my location.
* Suppose I want to save a byte in eeprom. For that I am using a ring buffer 
* of 4 bytes. So I am getting 4 times endurance of eeprom

* So for saving value in eeprom, we need two types of buffer 
* 1. Parameter Buffer : This is the intended value we want to store in EEPROM
* 2. Status Buffer: This buffer keeps track of my current location in buffer.
**************************************************************************/


/**************************************************************************
                           Author Information
***************************************************************************
Author : Shuvangkar Shuvo
email: sshuvo93[at]gmail.com
Dhaka, Bangladesh- March 2020
**************************************************************************/


#include "RingEEPROM.h"
#if defined(ESP32_V010) && !defined(ESP32_EEPROM)
#include "../../../../common_lib/FM24I2C/FM24I2C.h"
#else
#include <EEPROM.h>
#endif

// #define EEP_DEBUG
#ifdef EEP_DEBUG
#define debugEep(...) Serial.print(__VA_ARGS__)
#define debugEepln(...) Serial.println(__VA_ARGS__)
#else
#define debugEep(...)   __VA_ARGS__
#define debugEepln(...)  __VA_ARGS__
#endif

uint8_t _readCalcChecksum(int addr, uint16_t len);


RingEEPROM::RingEEPROM(int addrPtr, byte bufSz, byte paramSize)
{
  _initAddr = addrPtr; //First address of the buffer
  _bufSz = bufSz;      //Total number of buffer 
  _paramPacketSz = paramSize;// Total byte in a parameter packet
  //_clrStatusBuf();
}

RingEEPROM::RingEEPROM()
{
  //default parameters
  _initAddr = 0;
  _bufSz = 4;
  _paramPacketSz = 2;
}

RingEEPROM::RingEEPROM(int addrPtr)
{
	_initAddr = addrPtr;
	_bufSz = 4;
    _paramPacketSz = 2;
}

void RingEEPROM::begin(int addrPtr, byte bufSz, byte paramSize)
{
  _initAddr = addrPtr; //First address of the buffer
  _bufSz = bufSz;      //Total number of buffer 
  _paramPacketSz = paramSize;// Total byte in a parameter packet
}

void RingEEPROM::begin(byte bufSz, byte paramSize)
{
  _bufSz = bufSz;      //Total number of buffer 
  _paramPacketSz = paramSize;// Total byte in a parameter packet
}

/*************************************************************
* When status buffer becomes full, this method clears the status buffer
*************************************************************/
void RingEEPROM::_clrStatusBuf()
{
  uint8_t ptr = 0;
  debugEep("_initAddr: "); debugEepln(_initAddr); 
  for (byte i = 0; i < _bufSz; i++) {
#if defined(ESP32_V010)  && !defined(ESP32_EEPROM)
  #warning <-------FRAM ON------->
    ptr = 0;
    fram.write(_initAddr + i,(void*)&ptr,1);
#else
    EEPROM.write(_initAddr + i, 0);
#endif
  }
#if defined(ESP32_EEPROM)
  EEPROM.commit();
#endif
  debugEepln(F("Status Buf Cleared"));
  //Extra protection is needed here. after setting zero read and check
  // whether all values are zero.
}

/************************************************************
*This methods scan the status buffer and find out the current
*location to write
*************************************************************/
byte RingEEPROM::_getStatusPtr()
{
  byte ptr;
  byte i = 0;
  do
  {
#if defined(ESP32_V010) && !defined(ESP32_EEPROM)
    fram.read((_initAddr + i),(void*)&ptr,1);
#else
    ptr = EEPROM.read( _initAddr + i);
#endif
    // debugEep("ptr");
    // debugEep(ptr);
    // debugEep("  _getStatusPtr");
    // debugEepln(i);
  } while (++i && ptr && (i <= _bufSz));
  return i;
}

/************************************************************
*user call this methods to save the parameter packet
*************************************************************/
void RingEEPROM::savePacket(byte *dataBuf)
{
  int retryCount = 3;
  byte statusPtr = _getStatusPtr();
  if (statusPtr > _bufSz)
  {
     debugEepln(F("<<<<<<<<<Buf Full>>>>>"));
    _clrStatusBuf();
     //first value of status pointer is 1, zero indicates
     // the location is not used yet
    statusPtr = 1;
  }

  byte index = statusPtr - 1;
  //Status pointer address ends, param pointer address starts
  int paramInitAddr = _initAddr + _bufSz;
  _paramPtr = paramInitAddr + (index * _paramPacketSz);

  uint8_t checkSum = 0;
  debugEep(F("Data Save Addr: ")); debugEepln(_paramPtr);
  int addr;
  do
  {
    for (byte i = 0; i < _paramPacketSz; i++)
    {
      addr = _paramPtr + i;
#if defined(ESP32_V010) || !defined(ESP32_EEPROM)
      fram.write(addr,(void*)&dataBuf[i],1);
#else
      EEPROM.write(addr, dataBuf[i]);
#endif
      checkSum += (uint8_t)dataBuf[i];
    }
#if defined(ESP32_EEPROM)
  EEPROM.commit();
#endif
    // Serial.println("RingEEPROM> Checksum matched");
  }
  while(--retryCount && (_readCalcChecksum(_paramPtr, _paramPacketSz) != checkSum ));
  
  // if(_readCalcChecksum(_paramPtr, _paramPacketSz) == checkSum )
  // {
  //   Serial.println("RingEEPROM> Checksum matched");
  // }
  // else
  // {
  //   Serial.println("RingEEPROM> Checksum not matched");
  // }

#if defined(ESP32_V010) && !defined(ESP32_EEPROM)
  fram.write(_initAddr + index, (void*)&statusPtr, 1);
#else
  EEPROM.write(_initAddr + index, statusPtr); // update status pointer
#endif
#if defined(ESP32_EEPROM)
  EEPROM.commit();
#endif
}

/************************************************************
*user call this methods to read last saved value
*************************************************************/
void  RingEEPROM::readPacket(byte *dataBuf)
{
  byte statusPtr = _getStatusPtr();
  byte index = statusPtr - 2;
  int paramInitAddr = _initAddr + _bufSz;
  int paramPtr = paramInitAddr + (index * _paramPacketSz);
  debugEep(F("Data Read Addr: ")); debugEepln(paramPtr);
  //byte value;
#if defined(ESP32_V010) && !defined(ESP32_EEPROM)
  fram.read(paramPtr,(void*)dataBuf,_paramPacketSz);
#else
  for(byte i =0 ; i<_paramPacketSz; i++)
  {
    dataBuf[i] = EEPROM.read(paramPtr+i);
    //Serial.print(dataBuf[i]);Serial.print("  ");
  }
#endif
  //Serial.println();
}

/************************************************************
*This function retunrs the last address of the complete buffers. 
* So that use can know what is the end pointer for the eeprom object
*************************************************************/
uint16_t RingEEPROM::getBufLastAddr()
{
  uint16_t totalByte = _bufSz + (_bufSz*_paramPacketSz);
  totalByte = _initAddr + totalByte;
  return totalByte;
}

/************************************************************
*Return the current address of EEPROM parameter
*************************************************************/
uint16_t  RingEEPROM::getParamPtr()
{
	return _paramPtr;
}

void RingEEPROM::printStatusBuf()
{
  Serial.print(F("Status Buf: "));
  byte value;
  for (byte i = 0; i < _bufSz; i++)
  {
#if defined(ESP32_V010) && !defined(ESP32_EEPROM)

  fram.read(_initAddr + i,(void*)&value,1);
#else
    value = EEPROM.read( _initAddr + i);
#endif
    Serial.print(value); Serial.print("  ");
  }
  Serial.println();
}
void RingEEPROM::printArray(byte *data, byte len)
{
  for (byte i = 0; i < len; i++)
  {
    debugEep(*(data+i)); debugEep("  ");
  }
  debugEepln();
}

void RingEEPROM::populateStatus()
{
  byte value;
  for (byte i = 0; i < _bufSz - 3; i++)
  {
#if defined(ESP32_V010) && !defined(ESP32_EEPROM)
  value = i + 1;
  fram.write(_initAddr + i,(void*)&value,1);
#else
    EEPROM.write(_initAddr + i, i + 1);
#endif
  }
#if defined(ESP32_EEPROM)
  EEPROM.commit();
#endif
}


uint8_t _readCalcChecksum(int addr, uint16_t len)
{
  uint8_t cks = 0;
  uint8_t value;
  int addrLen = addr + len;
  for(int i = addr; i < addrLen; i++)
  {
#if defined(ESP32_V010) && !defined(ESP32_EEPROM)
    fram.read(i,(void*)&value,1);
    cks += value;
#else
    cks += EEPROM.read(i);
#endif
  }
  return cks;
}

