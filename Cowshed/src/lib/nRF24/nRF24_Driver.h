/*
   NRF24_Driver.h

    Created on: Aug 27, 2020
        Author: Shuvangkar
*/

#ifndef NRF24_DRIVER_H_
#define NRF24_DRIVER_H_

#if defined(ARDUINO_ARCH_AVR)
    #include <Arduino.h>
    #if defined(PROD_BUILD)
        #include "../arduinoCwrapper/Serial.h"
        #include "../arduinoCwrapper/spi_driver.h"
    #else
        #include "spi_driver.h"
        #include "Serial.h"
    #endif
#elif defined(ARDUINO_ARCH_SAM)
    #include <Arduino.h>
#elif defined(__MSP430G2553__)
    #include <msp430.h>
    #include "mspDriver.h"
#else
    #error "nRF24_DRIVER did not find chip architecture "
#endif
// Default Core Library
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nRF24_User_Conf.h"
#include "nRF24_Registers.h"


#define nrf_csn_low()     (*_csnPort &= ~(1<< _csnPin))
#define nrf_csn_high()    (*_csnPort |=  (1<< _csnPin))

#define nrf_ce_high()  (*_cePort |= (1 << _cePin));
#define nrf_ce_low()   (*_cePort &= ~(1 << _cePin));

#if defined(ARDUINO_ARCH_AVR)
    #define nrf_ce_read()  (*(_cePort-2) & (1 << _cePin));
#elif defined(__MSP430G2553__)
    #define nrf_ce_read()  (*(_cePort-1) & (1 << _cePin));
#else
 #error "chip support not found"
#endif

uint8_t read_register(uint8_t addr);
void write_register(uint8_t addr, uint8_t data);

void write_bytes_in_register(uint8_t addr, uint8_t *payload, uint8_t len);
uint8_t *read_bytes_in_register(uint8_t addr, uint8_t *bucket, uint8_t len);

void set_reg_bit(uint8_t reg, uint8_t bitMask);
void clear_reg_bit(uint8_t reg, uint8_t bitMask);

void nrf_flush_tx();
void nrf_flush_rx();

void nrf_set_tx_addr(uint8_t *addr, uint8_t len);
void nrf_set_rx_addr(uint8_t pipe, uint8_t *addr, uint8_t len);


void nrf_set_addr_width(uint8_t width);
void nrf_set_tx_dbm_speed(uint8_t);


#define dlp_enable() write_register (RF24_FEATURE,EN_DLP)
#define dlp_disable() clear_reg_bit(RF24_FEATURE,EN_DLP)

void dynpd_disable(uint8_t pipe);
void dynpd_enable(uint8_t pipe);

void nrf_debug_register(uint8_t addr);
void nrfDebugBuffer(void *ptr, uint8_t len);

/*******Extern global vars******/
extern volatile uint8_t nrf_status;
extern volatile bool _nrfDebug;
extern volatile uint8_t *_cePort;
extern volatile uint8_t _cePin;
extern volatile uint8_t *_csnPort;
extern volatile uint8_t _csnPin;

#endif /* NRF24_DRIVER_H_ */


// void nrf_write_tx_payload(uint8_t *data, uint8_t len);
// uint8_t *nrf_read_rx_payload(uint8_t *data, uint8_t len);
// void nrf_start_transmit();
// void nrf_start_receive();

// void nrf_clr_tx_int();
// void nrf_clr_rx_int();

// void nrf_power_up_tx();
// void nrf_power_up_rx();
// void nrf_power_down();

// bool is_tx_not_empty();
// void nrf_enable_ack(uint8_t pipe);
// void dynPayload(uint8_t pipe, bool onOff);
// void nrf_enable_irq_int();