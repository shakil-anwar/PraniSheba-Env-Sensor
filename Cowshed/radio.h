#ifndef _RADIO_H_
#define _RADIO_H_
#include "Arduino.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>




void radio_begin(void);
extern RF24 radio;
#endif
