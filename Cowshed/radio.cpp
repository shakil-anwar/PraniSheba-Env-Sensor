#include "radio.h"
#include "pin.h"
#include "dataSchema.h"
#include "Obj.h"

//Flash flash(FLASH_CS);

int retryCount;
bool rf_send_success;

//RingEEPROM myeepRom(0x00);

//MemQ memQ(256, 1000);

uint8_t buf2[5];
void IsrNrf();
bool rf_led_state =HIGH;


void radio_begin()
{
//  memQ.attachFlash(&flash, (void**)&buffer.flashPtr, sizeof(payload_t),TOTAL_PAYLOAD_BUFFERS/2);
//  memQ.attachEEPRom(&myeepRom, 4);
//  memQ.reset();
  rf_send_success = false;
  nrf_send_success = false;
  retryCount = 0;

  
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


void IsrNrf()
{
  Serial.println("=====>NRF IRQ Triggered");
  uint8_t rfStatus  = read_register(RF24_STATUS);
  if(rfStatus && TX_DS)
  {
    Serial.println("NRF Send Success");
    rf_led(rf_led_state = !rf_led_state);
    rf_send_success = true;
  }else  {
    retryCount = 16;
    Serial.println("NRF Send Failed");
    rf_led(LOW);
  }
  write_register(RF24_STATUS,rfStatus | TX_DS | MAX_RT);
//  rf_led(HIGH);
}
