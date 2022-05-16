#include "FM24I2C.h"
#include <Wire.h>

FM24I2C fram(0x50);

FM24I2C::FM24I2C(int id_addr) {
  id=id_addr;
}

FM24I2C::~FM24I2C() {}


// void FM24I2C::pack(int addr, void* data, int len) {
//   Wire.beginTransmission(id);
//   Wire.write(addr);
//   Wire.write((byte*)data,len);
//   Wire.endTransmission(true);
//   delay(5);
// }

int FM24I2C::read(int addr, void* data, int len) 
{
  int rc;
  byte *p;
  Wire.beginTransmission(id);
  Wire.write(addr);
  Wire.endTransmission();
  delay(5);
  Wire.requestFrom(id,len);
  for (rc=0, p=(byte*)data; Wire.available() && rc < len; rc++, p++) {
    *p=Wire.read();
  }
  Wire.endTransmission();
  return(rc);
}


void FM24I2C::write(int addr, void* data, int len) 
{
  uint8_t *dataBuf = (uint8_t *)data;
  // Serial.print("Address: "); Serial.print(addr);
  // Serial.print(" Data Len: ");Serial.println(len);
  int res = len / 8;
  int rem = len % 8;
  int loopCount = 0;
  int dataLen = 0;
  if (len == 0)
  {
    return;
  }

  if(rem != 0)
  {
    loopCount = res + 1;
  }
  else
  {
    loopCount = res;
  }
  for (int i = 0; i< loopCount; i++)
  {
    dataLen = i*8;
    if(i == res && rem == 0)
    {
      break;
    }
    Wire.beginTransmission(id);
    Wire.write(addr+dataLen);
    if(i==res)
    {
      Wire.write((byte*)(dataBuf+dataLen),rem);
    }
    else
    {
      Wire.write((byte*)(dataBuf+dataLen),8);
    }
    Wire.endTransmission(true);
    delay(5);
  }
}
