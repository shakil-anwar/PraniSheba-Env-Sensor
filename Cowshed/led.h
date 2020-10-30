#ifndef _H_LED_
#define _H_LED_
#include "Arduino.h"

void led_begin();
void led_blink();
void set_led_level(uint8_t col,uint8_t level);

#endif
