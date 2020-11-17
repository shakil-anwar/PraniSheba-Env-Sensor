#include "radio.h"
#include "pin.h"
#include "dataSchema.h"
#include "Obj.h"

#define QUERY_PIPE 5
void txIsr(void);
void rxIsr(void);
void maxRtIsr(void);
void IsrNrf();
bool rf_led_state = HIGH;

uint8_t pipeAddr[6][5] =
{
  {1, 2, 3, 4, 5},
  {2, 2, 3, 4, 5},
  {3, 2, 3, 4, 5},
  {4, 2, 3, 4, 5},
  {5, 2, 3, 4, 5},
  {6, 2, 3, 4, 5}
};
void radio_begin()
{
  nrfBegin(SPEED_2MB, POWER_ZERO_DBM);
  nrfSetTx(pipeAddr[1], true);
  nrfTXMode();
  nrfPowerUp();

  nrfSetQuery(QUERY_PIPE, pipeAddr[QUERY_PIPE]);
  nrfSetIrqs(txIsr, rxIsr, maxRtIsr);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), nrfIrq, FALLING );
}

void txIsr(void)
{
  nrfClearTxDs();
}

void rxIsr(void)
{
  uint8_t pipe = pipeAvailFast();
  nrfClearRxDr();
}

void maxRtIsr(void)
{
  nrfClearMaxRt();
//  nrf_flush_rx();
  nrf_flush_tx();
}



void IsrNrf()
{
  Serial.println("=====>NRF IRQ Triggered");
  uint8_t rfStatus  = read_register(RF24_STATUS);
  if (rfStatus && TX_DS)
  {
    Serial.println("NRF Send Success");
    rf_led(rf_led_state = !rf_led_state);
    rf_send_success = true;
  } else  {
    retryCount = 16;
    Serial.println("NRF Send Failed");
    rf_led(LOW);
  }
  write_register(RF24_STATUS, rfStatus | TX_DS | MAX_RT);
  //  rf_led(HIGH);
}
