#include "radio.h"
#include "pin.h"
#include "dataSchema.h"

Flash flash(FLASH_CS);


RingEEPROM myeepRom(0x00);

MemQ memQ(256, 1000);

uint8_t buf2[5];
void IsrNrf();
bool rf_led_state =HIGH;


void radio_begin()
{
  memQ.attachFlash(&flash, (void**)&buffer.flashPtr, sizeof(payload_t),TOTAL_PAYLOAD_BUFFERS/2);
  memQ.attachEEPRom(&myeepRom, 4);
  memQ.reset();
  nrf_send_success = false;

  
  if(!buffer.nrfPtr)
  {
    Serial.println(F("Buf Ptr not Initializer"));
  }
  
  nrf_begin();
  nrf_common_begin();
  nrf_tx_begin();
  attachInterrupt(digitalPinToInterrupt(3), IsrNrf, FALLING );
  read_bytes_in_register(RF24_TX_ADDR, buf2, 5);
  Serial.print("RX Address");
  Serial.println((char)buf2, HEX);
//  printBuffer(buf2, 5);
  nrfConfigPrint();

  write_register(RF24_STATUS, RX_DR);
}


void readPayload()
{
  Serial.print(F("Payload Receiving:  "));Serial.println(second());
  uint8_t *payload = (uint8_t*)&buffer.nrfPtr[buffer.pIndex];
  sensor_t *sensorPtr = getSensorsData();
  memset(payload,'\0',MAX_PAYLOAD_BYTES);
  memcpy(payload,(uint8_t*)sensorPtr,sizeof(sensor_t));

  sensor_t *bptr = (sensor_t*)&buffer.nrfPtr[buffer.pIndex];
  
  Serial.print(F("bufIndex :")); Serial.print(buffer.tIndex);
  Serial.print(F(" | index :")); Serial.print(buffer.pIndex);
  Serial.print(F(" | DataIndex :"));Serial.println(bptr -> id);
  
  buffer.pIndex++;
  buffer.tIndex++;
  if (buffer.tIndex >= TOTAL_PAYLOAD_BUFFERS / 2)
  {
    if (buffer.tIndex == TOTAL_PAYLOAD_BUFFERS / 2)
    {
      buffer.nrfPtr   = (payload_t*)&buffer.payload[TOTAL_PAYLOAD_BUFFERS / 2];
      buffer.flashPtr = (payload_t*)&buffer.payload[0];
      buffer.pIndex = 0;
    }
    else if (buffer.tIndex == TOTAL_PAYLOAD_BUFFERS)
    {
      buffer.nrfPtr   = (payload_t*)&buffer.payload[0];
      buffer.flashPtr = (payload_t*)&buffer.payload[TOTAL_PAYLOAD_BUFFERS / 2];
      buffer.pIndex   = 0;
      buffer.tIndex   = 0;
    }
  }
  Serial.println("");
  for(int j=0;j<MAX_PAYLOAD_BYTES ; j++){
    Serial.print((char)payload[j], HEX);
    Serial.print(" ");
  }
  Serial.println("");

//  nrf_send(payload);

//  printSensor(sensorPtr);
  //handle nrf data sending here 
}


void IsrNrf()
{
  Serial.println("=====>NRF IRQ Triggered");
  uint8_t rfStatus  = read_register(RF24_STATUS);
  if(rfStatus && TX_DS)
  {
    Serial.println("NRF Send Success");
    rf_led(rf_led_state = !rf_led_state);
    nrf_send_success = true;
  }else  {
    Serial.println("NRF Send Failed");
  }
  write_register(RF24_STATUS,rfStatus | TX_DS | MAX_RT);
}
