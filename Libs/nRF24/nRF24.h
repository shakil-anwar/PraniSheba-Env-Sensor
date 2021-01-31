/*
    nrf24.h

    Created on: Aug 31, 2020
    Author: sshuv
*/
#ifndef _NRF24_H_
#define _NRF24_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "nRF24_Driver.h"
#include "nRF24_Registers.h"


typedef enum nrf_irq_state_t
{
  NRF_WAIT = 429,
  NRF_SUCCESS = 200,
  NRF_RECEIVED = 202,
  NRF_FAIL = 404
}nrf_irq_state_t;


typedef enum nrf_mode_t
{
  POWER_DOWN,
  STANDBY1,
  STANDBY2,
  RXMODE,
  TXMODE
}nrf_mode_t;

typedef void (*isrPtr_t)(void);


void nrfSetPin(uint8_t *cePort, uint8_t cePin, uint8_t *csnPort, uint8_t csnPin);
void nrfBegin(air_speed_t speed, power_t power, uint32_t spiSpeed);

void nrfSetPipe(uint8_t pipe, uint8_t *addr, bool ackFlag);
void nrfRxStart();

void nrfEnableAck(uint8_t pipe); // added pipe no as parameter, pipe 6 means set autoack on all pipes


void nrfSetTx(uint8_t *addr, bool ackFlag);
void nrfTXStart();
void nrfStartTransmit();


bool nrfIsRunning();

bool nrfIsRxEmpty();
bool nrfRxFifiFull();
uint8_t nrfReadStatus();
bool nrfIntStatus(uint8_t bitPos);
uint8_t pipeAvailable();
uint8_t pipeAvailFast();
bool nrfTxIsFifoEmpty();
nrf_mode_t nrfWhichMode();



void nrfWrite(const uint8_t *data, uint8_t len);
void nrfSend(const uint8_t *data, uint8_t len);
bool nrfAck();
bool nrfAckSend(const uint8_t *data, uint8_t len);

// bool nrfSend(const uint8_t *data, uint8_t len);
void nrfWrite96(uint8_t *data, uint8_t len);
void nrfWriteAck(uint8_t *data, uint8_t len);


void nrfSetIrqs(isrPtr_t txIsr, isrPtr_t rxIsr, isrPtr_t maxRtIsr);
void nrfIrq();


nrf_irq_state_t waitAck();

uint8_t *nrfRead(uint8_t *buffer, uint8_t len);

void nrfSetPldAck();
void nrfClearIrq();

void nrfRxTxToStandy1();
void nrfRestorToRxTx();

void nrfSetCh(uint8_t ch);
uint8_t nrfPayloadLen();

#define nrfClearTxDs() write_register(RF24_STATUS, TX_DS)
#define nrfClearRxDr() write_register(RF24_STATUS, RX_DR)
#define nrfClearMaxRt() write_register(RF24_STATUS, MAX_RT)

void nrfDebugPrint();

void nrfReUseTX();

void nrfDebug(bool debugFlag);

void nrfSetMillis(uint32_t (*ms)());

#define nrfPowerUp()    ({\
                        set_reg_bit(RF24_CONFIG, PWR_UP);\
                        delay(2);\
                        })
#define nrfPowerDown()  clear_reg_bit(RF24_CONFIG, PWR_UP)
#define nrfStandby1()   ({\
                        set_reg_bit(RF24_CONFIG, PWR_UP);\
                        delay(2);\
                        })

#define nrfRXMode()     set_reg_bit(RF24_CONFIG,RX_MODE)
#define nrfTXMode()     clear_reg_bit(RF24_CONFIG,RX_MODE)



#if defined(ARDUINO_ARCH_AVR)
  #define delay_us(us)     delayMicroseconds(us);
#else
  #define delay_us(us)     __delay_cycles(us);
#endif


extern nrf_irq_state_t _nrfIrqState;
extern bool isTXActive;
extern uint32_t (*_Millis)(void);

#ifdef __cplusplus
}
#endif

#endif
