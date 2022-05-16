#ifndef _H_LED_
#define _H_LED_
// #include "../IoT.h"
#include "device.h"
#if !defined(SOFTWARE_I2C)

void led_begin();
void led_blink();
void led_set_level(uint8_t col,uint8_t level);
void ledTestAll();
void rf_led(bool state);

void updateDisplay();
#endif
#endif
