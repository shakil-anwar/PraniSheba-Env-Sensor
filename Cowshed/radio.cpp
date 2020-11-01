#include "radio.h"

uint8_t buf2[5];
void IsrNrf();
bool rf_led_state =HIGH;


void radio_begin()
{
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
    nrf_send_success = true;
  }else  {
    Serial.println("NRF Send Failed");
  }
  write_register(RF24_STATUS,rfStatus | TX_DS | MAX_RT);
}
