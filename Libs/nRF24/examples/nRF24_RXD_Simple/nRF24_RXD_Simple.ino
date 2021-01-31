#include "nRF24.h"

/*include Gas Sensor*/
//#define FLASH_CS  8
//#define NRF_IRQ_PIN 3
/****Base Station*****/
#define FLASH_CS    5 
#define NRF_IRQ_PIN 2

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
  pinMode(FLASH_CS, OUTPUT);
  digitalWrite(FLASH_CS , HIGH);

  nrfSetPin(&PORTB,2, &PORTB, 1);
  nrfBegin(SPEED_2MB, POWER_ZERO_DBM, 1000000);
  nrfSetPipe(1, pipe1Addr, true);

  nrfSetIrqs(txIsr, rxIsr, maxRtIsr);

  pinMode(NRF_IRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(NRF_IRQ_PIN), nrfIrq, FALLING );
  nrfPowerDown();
  nrfStandby1();
  nrfRxStart();//goes to RX mode.
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
