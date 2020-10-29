#include "led.h"
#include "pin.h"

#include <ShiftRegister74HC595.h>

void col_one(uint8_t level);
void col_two(uint8_t level);
void col_three(uint8_t level);
void col_four(uint8_t level);

// parameters: (number of shift registers, data pin, clock pin, latch pin)
ShiftRegister74HC595<TOTAL_SR> sr(SR_DATA, SR_CLOCK, SR_LATCH);

#define COL1_START  1
#define COL1_END    5
#define COL3_START  11
#define COL3_END    15



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

  uint8_t led_array[5] = {6, 7, 8, 9, 10};
  for (uint8_t i = 0; i < 5; i++)
  {
    if (i < level)
    {
      sr.set(led_array[i], HIGH);
    }
    else
    {
      sr.set(led_array[i], LOW);
    }
  }
}

void col_three(uint8_t level)
{
  for (uint8_t i = COL3_START ; i <= COL3_END; i++)
  {
    if (i < (COL3_START + level))
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

  uint8_t led_array[5] = {16,17,18,19,20};
  for (uint8_t i = 0; i < 5; i++)
  {
    if (i < level)
    {
      sr.set(led_array[i], HIGH);
    }
    else
    {
      sr.set(led_array[i], LOW);
    }
  }
}



void set_led_level(uint8_t col,uint8_t level)
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
