#include "nRF24.h"

//#define BOARD_GAS_SENSOR
//#define BOARD_TEST
#define BOARD_BASE_STATION

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
  #define NRF_CE_PORT   PORTB
  #define NRF_CE_PIN    PB2 
  #define NRF_CSN_PORT  PORTB
  #define NRF_CSN_PIN   PB1
  #define NRF_IRQ_PIN   2
  #define BUS_FLASH_PIN 5
#else
#error "Define a supported board"
#endif


void txIsr(void);
void rxIsr(void);
void maxRtIsr(void);

uint8_t pipe0Addr[] = {1, 2, 3, 4, 5};
uint8_t pipe1Addr[] = {2, 2, 3, 4, 5};
uint8_t pipe2Addr[] = {3, 2, 3, 4, 5};
uint8_t pipe3Addr[] = {4, 2, 3, 4, 5};
uint8_t pipe4Addr[] = {5, 2, 3, 4, 5};
uint8_t pipe5Addr[] = {6, 2, 3, 4, 5};

uint8_t temp[32];

uint32_t packetCounter = 0;
void setup()
{
  Serial.begin(250000);
  pinMode(BUS_FLASH_PIN, OUTPUT);
  digitalWrite(BUS_FLASH_PIN , HIGH);

  nrfSetTimers(millis,NULL);
  nrfSetPin(&NRF_CE_PORT, NRF_CE_PIN, &NRF_CSN_PORT, NRF_CSN_PIN);
  nrfBegin(SPEED_2MB, POWER_ZERO_DBM, 1000000);
  nrfSetPipe(1, pipe1Addr, true);

  nrfSetIrqs(txIsr, rxIsr, maxRtIsr);
  nrfDebug(true);

  pinMode(NRF_IRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(NRF_IRQ_PIN), nrfIrq, FALLING );
  nrfPowerDown();
  nrfStandby1();
  nrfRxStart();    //goes to RX mode.
  Serial.println(F("RX Setup done"));
}

void loop()
{
  // put your main code here, to run repeatedly:

}

void rxIsr(void)
{
  uint8_t pipe = pipeAvailFast();
  Serial.print(F("PIPE # : ")); Serial.println(pipe);
  int8_t nrfMaxRfBuffer = 3;//total rx buffer in nrf
  if (pipe < 6)
  {
    do
    {
      nrfRead(temp, sizeof(temp));
      printBuffer(temp, sizeof(temp));
      //      nrfDebugPrint();
    } while (!nrfIsRxEmpty() && --nrfMaxRfBuffer);
  }

}
void txIsr(void)
{
  nrfRxStart();
}
void maxRtIsr(void)
{
  nrf_flush_rx();
  nrf_flush_tx();
  nrfRxStart();
}

void printBuffer(uint8_t *buf, uint8_t len)
{
  for (uint8_t i = 0; i < len; i++)
  {
    Serial.print(buf[i]); Serial.print(' ');
  }
  Serial.println();
}
