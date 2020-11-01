#include "led.h"
#include "pin.h"

#include <ShiftRegister74HC595.h>

void col_one(uint8_t level);
void col_two(uint8_t level);
void col_three(uint8_t level);
void col_four(uint8_t level);

// parameters: (number of shift registers, data pin, clock pin, latch pin)
ShiftRegister74HC595<TOTAL_SR> sr(SR_DATA, SR_CLOCK, SR_LATCH);

#define COL1_START  5
#define COL1_END    1
#define COL2_START  10
#define COL2_END    6
#define COL3_START  15
#define COL3_END    11
#define COL4_START  20
#define COL4_END    16



void led_begin()
{
  pinMode(SR_PWM, OUTPUT);
  digitalWrite(SR_PWM, LOW);
  //analogWrite(SR_PWM,230);
  sr.setAllLow(); // set all pins LOW
}

void col_one(uint8_t level)
{
  for (uint8_t i = COL1_START; i >= COL1_END ; i--)
  {
    if ((i > (COL1_START - level)))
    {
      sr.set(i, HIGH);
    }
    else
    {
      sr.set(i, LOW);
    }
  }
}


void col_two(uint8_t level)
{
  //  column 2 pin sequence
  //  led 1 -> 14
  //  led 2 -> 15
  //  led 3 -> 0
  //  led 4 -> 1
  //  led 4 -> 2
  for (uint8_t i = COL2_START; i >= COL2_END ; i--)
  {
    if ((i > (COL2_START - level)))
    {
      sr.set(i, HIGH);
    }
    else
    {
      sr.set(i, LOW);
    }
  }
}

void col_three(uint8_t level)
{
  for (uint8_t i = COL3_START; i >= COL3_END ; i--)
  {
    if ((i > (COL3_START - level)))
    {
      sr.set(i, HIGH);
    }
    else
    {
      sr.set(i, LOW);
    }
  }
}

void col_four(uint8_t level)
{
  //  column 2 pin sequence
  //  led 1 -> 20
  //  led 2 -> 21
  //  led 3 -> 22
  //  led 4 -> 23
  //  led 4 -> 8
  for (uint8_t i = COL4_START; i >= COL4_END ; i--)
  {
    if ((i > (COL4_START - level)))
    {
      sr.set(i, HIGH);
    }
    else
    {
      sr.set(i, LOW);
    }
  }
}



void led_set_level(uint8_t col,uint8_t level)
{

  switch(col)
  {
    case 1:
      col_one(level);
    break;
    case 2:
      col_two(level);
    break;
    case 3:
      
      col_three(level);
    break;
    case 4:
      col_four(level);
    break;
  }
}

void led_blink()
{
  //  sr.setAllHigh(); // set all pins HIGH
  //  delay(500);

  //  sr.setAllLow(); // set all pins LOW
  //  delay(500);
  for (int i = 0; i < 21; i++)
  {
    sr.set(i, HIGH);
    delay(1000);
    sr.set(i, LOW);
  }
  //   sr.set(15, HIGH);
}

void rf_led(bool state)
{
  sr.set(0,state);
}

void ledTestAll()
{
    for (int i = 1; i < 5; i++)
  {
    for (int j = 1; j <= 5; j++)
    {
      led_set_level(i, j);
      delay(1000);
    }
  }
}
