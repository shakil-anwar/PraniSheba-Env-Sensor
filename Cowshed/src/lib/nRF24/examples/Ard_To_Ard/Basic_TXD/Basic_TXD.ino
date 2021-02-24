#include "nRF24.h"
#define PAYLOAD_LEN 32

#define BOARD_GAS_SENSOR
//#define BOARD_TEST

#if defined(BOARD_GAS_SENSOR)
  #define NRF_CE_PORT   PORTB
  #define NRF_CE_PIN    PB2
  #define NRF_CSN_PORT  PORTB
  #define NRF_CSN_PIN   PB1
  #define NRF_IRQ_PIN   3
  #define BUS_FLASH_PIN 8
#elif defined(BOARD_TEST)
  #define NRF_CE_PORT   PORTB
  #define NRF_CE_PIN    PB1
  #define NRF_CSN_PORT  PORTB
  #define NRF_CSN_PIN   PB2
  #define NRF_IRQ_PIN   2
  #define BUS_FLASH_PIN 6
#elif defined(BOARD_BASE_STATION)
  
#else
#error "Define a supported board"
#endif

void txIsr(void);
void rxIsr(void);
void maxRtIsr(void);

uint8_t payload[PAYLOAD_LEN];
uint8_t pipe1Addr[] = {2, 2, 3, 4, 5};


uint32_t packetCounter = 0;

uint32_t waitPrevMillis = 0;

void setup()
{
  Serial.begin(250000);
  pinMode(BUS_FLASH_PIN, OUTPUT);
  digitalWrite(BUS_FLASH_PIN , HIGH);

  nrfSetTimers(millis,NULL);
  nrfSetPin(&NRF_CE_PORT, NRF_CE_PIN, &NRF_CSN_PORT, NRF_CSN_PIN);
  nrfBegin(SPEED_2MB, POWER_ZERO_DBM, 1000000);
  nrfSetIrqs(txIsr, rxIsr, maxRtIsr);
  nrfSetTx(pipe1Addr, true);

  pinMode(NRF_IRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(NRF_IRQ_PIN), nrfIrq, FALLING );
  delay(2000);

  nrfPowerDown();
  nrfStandby1();
  nrfTXMode();
  Serial.println("TX Setup Done");
}

void loop()
{
  generateData(payload);
  printBuffer(payload,32);
  bool ok = nrfAckSend(payload,32);
  Serial.print(F("NRF Send : "));Serial.println(ok);
  delay(5000);
}


void txIsr(void)
{
  nrfClearTxDs();
}

void rxIsr(void)
{
  nrfClearRxDr();
}

void maxRtIsr(void)
{
  nrfClearMaxRt();
  nrf_flush_rx();
  nrf_flush_tx();

}

void generateData(uint8_t *buf)
{
  uint8_t *p = buf;
  for (uint8_t i = 0; i < PAYLOAD_LEN; i++)
  {
    p[i] = random(100, 250);
  }
}

void printBuffer(uint8_t *buf, uint8_t len)
{
  for (uint8_t i = 0; i < len; i++)
  {
    Serial.print(buf[i]); Serial.print(' ');
  }
  Serial.println();
}
