#include "led.h"

#if !defined(SOFTWARE_I2C)

void col_one(uint8_t level);
void col_two(uint8_t level);
void col_three(uint8_t level);
void col_four(uint8_t level);

// parameters: (number of shift registers, data pin, clock pin, latch pin)
ShiftRegister74HC595<TOTAL_SR> sr(SR_DATA, SR_CLOCK, SR_LATCH);

#if !defined(ESP32_V010)
#define COL1_START  5
#define COL1_END    1
#define COL2_START  10
#define COL2_END    6
#define COL3_START  15
#define COL3_END    11
#define COL4_START  20
#define COL4_END    16

#else
#define SENSOR1_START   1
#define SENSOR1_END     3
#define SENSOR2_START   4
#define SENSOR2_END     6
#define SENSOR3_START   7
#define SENSOR3_END     9
#define SENSOR4_START   10
#define SENSOR4_END     12
#define SENSOR5_START   13
#define SENSOR5_END     15
#endif



void led_begin()
{
#if defined(SR_PWM)
  pinMode(SR_PWM, OUTPUT);
  digitalWrite(SR_PWM, LOW);
#endif
  //analogWrite(SR_PWM,230);
  sr.setAllLow(); // set all pins LOW
  rf_led(true);
}


#if !defined(ESP32_V010)

void col_one(uint8_t level)
{
  for (uint8_t i = COL1_START; i >= COL1_END ; i--)
  {
    if ((i < (COL1_END + level)))
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
    if ((i < (COL2_END + level)))
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
    if ((i < (COL3_END + level)))
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
    if ((i < (COL4_END + level)))
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

void updateDisplay()
{
  struct gasSensor_t *sensorPtr = getSensorsData(&sensor);
//  printSensor(sensorPtr);
  uint8_t level;
  //humidity
  level = (uint8_t)sensorPtr -> temp;
  level = level / 10;
  led_set_level(1, level) ;

  //Temperature
  level = (uint8_t)sensorPtr -> hum;
  level = level / 20;
  led_set_level(2, level);

  //Methane
  level = (uint8_t)sensorPtr -> methane;
  level = level / 5;
  led_set_level(3, level);

  //Ammonia
  level = (uint8_t)sensorPtr -> ammonia;
  level = level / 4;
  led_set_level(4, level);
//  readPayload();
}


#else


void led_set_level(uint8_t sensorNo,uint8_t level)
{
  uint8_t maxLevel = 3;
  if(level > maxLevel)
  {
    level = maxLevel;
  }

  if(level == 0)
  {
    level = 1;
  }

  if(sensorNo ==2)
  {
    level = 4 - level;
  }

  if(sensorNo>0)
  {
    //uint8_t ledActivePin = sensorNo*3-level+1;
    uint8_t ledActivePin = (sensorNo-1)*3+level;
    Serial.print("led level: "); Serial.println(level);
    Serial.print("led pin: "); Serial.println(ledActivePin);
    for(uint8_t ledPin =(sensorNo-1)*3+1 ;ledPin<=((sensorNo-1)*3+3); ledPin++)
    {
    if(ledPin == ledActivePin)
    {
      Serial.print("led Active pin: "); Serial.println(ledPin);
      sr.set(ledPin, HIGH);
    }
    else
    {
      sr.set(ledPin, LOW);
    }
    }
  }
}

void updateDisplay()
{
  struct gasSensor_t *sensorPtr = getSensorsData(&sensor);
//  printSensor(sensorPtr);
  uint8_t level;
  //Temperature
  level = (uint8_t)sensorPtr -> temp;
  level = 30;
  level = level / 15; // 15
  led_set_level(1, level) ;

  //Humidity
  level = (uint8_t)sensorPtr -> hum;
  level = 80;
  level = level / 30; //30
  led_set_level(2, level);

  //Ammonia
  level = (uint8_t)sensorPtr -> ammonia;
  level = 30;
  level = level / 8; //8
  led_set_level(3, level);

  //Methane
  level = (uint8_t)sensorPtr -> methane;
  level = 30;
  level = level / 10; //10
  led_set_level(4, level);

  //Gas Sensor 3
  //level = (uint8_t)sensorPtr -> ammonia;
  level = 2;
  level = level / 1; //8
  led_set_level(5, level);

//  readPayload();
}
void ledTestAll()
{
    for (int i = 0; i <= 15; i++)
  {
    sr.set(i,HIGH);
    delay(2000);
    sr.set(i,LOW);
    delay(500);
  }
}
void ledalllow()
{
  for (int i = 0; i <= 15; i++)
  {
    sr.set(i,LOW);
    delay(100);
  }
}
#endif  //ESP32_V010


void rf_led(bool state)
{
  sr.set(0,state);
}

#endif  //SOFTWARE_I2C