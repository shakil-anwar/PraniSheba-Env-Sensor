#ifndef _H_LED_
#define _H_LED_
#include "Arduino.h"

void led_begin();
void led_blink();
void led_set_level(uint8_t col,uint8_t level);
void ledTestAll();

#endif
