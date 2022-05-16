#include "utility.h"
#include "./device_ps/pin.h"
#include "../common_lib/FM24I2C/FM24I2c.h"
#include "../common_lib/iotStructure/iotStructure.h"

#define REF_VOLTAGE 1.1




bool deviceHasRtc = true;
#if defined(ESP32_V010)
void eepromBegin(uint16_t eepromSize)
{
  EEPROM.begin(eepromSize);
}
#endif


float getRailVoltage() {
 #if defined(ESP32_V010)
  float railVolt;
  railVolt = (float)analogRead(VDD_PIN);
  railVolt = railVolt/4095;
  railVolt = (railVolt*3.3 + 0.18)*3;
  return railVolt;
#else
  ADMUX |= (1 << REFS0) | (1 << MUX3) | (1 << MUX2) | (1<< MUX1);
  ADMUX &= ~(1<<MUX0) & ~(1<<REFS1);
  delay(3); //delay for 3 milliseconds
  ADCSRA |= (1<< ADSC); // Start ADC conversion
  while (bit_is_set(ADCSRA,ADSC)); //wait until conversion is complete
  int result = ADCL; //get first half of result
  result |= ADCH<<8; //get rest of the result
  float railVolt = (REF_VOLTAGE / result)*1024; //Use the known REF_VOLTAGE to calculate rail voltage
  return railVolt;
#endif
}

void printRailVoltage()
{
  float vol= getRailVoltage();
  Serial.print("Rail Voltage: ");
  Serial.println(vol);
}


void EEPROMRead(uint32_t addr, uint8_t *buf, uint16_t len)
{

  uint16_t eepAddr = (uint16_t)addr;
  uint8_t *ptr = buf;
  Serial.print(F("EEPROM>R>: ")); Serial.println(eepAddr);
#if defined(FRAM_V010)
  fram.read(addr,(void *)buf,len);
#else
  for (uint16_t i = 0 ; i < len; i++)
  {
    *(ptr + i) = EEPROM.read(eepAddr + i);
    // Serial.print( *(ptr + i)); Serial.print(F("  "));
  }
#endif
  // Serial.println();
}

void EEPROMUpdate(uint32_t addr, uint8_t *buf, uint16_t len)
{

  uint16_t eepAddr = (uint16_t)addr;
  uint8_t *ptr = buf;
  Serial.print(F("EEPROM>W>: ")); Serial.println(eepAddr);
#if defined(FRAM_V010)
  fram.write(addr,(void *)buf,len);
#else
  for (uint16_t i = 0; i < len; i++)
  {
    #if defined(ESP32_V010)
    EEPROM.write(eepAddr + i, *(ptr + i));
    // Serial.print( *(ptr + i)); Serial.print(F("  "));
    EEPROM.commit();
    #else
    EEPROM.update(eepAddr + i, *(ptr + i));
    // Serial.print( *(ptr + i)); Serial.print(F("  "));
    #endif
  }
#endif
  // Serial.println();
}

#if defined(ESP32_V010)
unsigned long majorityElement(unsigned long *values, int8_t dataLength)
{
  int8_t counter = 0;                            // initializing counter
  unsigned long candidate = *values;                  // considering first element as candidate majority element
                             // max frequency of the candidate is zero
  int8_t i = 0;
  while (i < dataLength) 
  { // finding the possible candidate
    if (counter == 0) 
    {
      candidate = *(values + i);
    }
    if (*(values + i) == candidate) 
    {// if matches with previous element, counter will increment
      counter++;
    } 
    else 
    {
      counter --;
    }
    i++;
  }
  i = 0;
  int8_t maxCount = 0;
  while ( i < dataLength)
  { // finding the frequency of the candidate
    if (*(values + i) == candidate)
    {
      maxCount++;
    }
    i++;
  }
  if (maxCount > dataLength / 2)
  { // if frequency > dataLength/2 --> this is what we want
    return candidate;
  } 
  else
  {
    return 0;   // means no candidate found
  }
}


#if defined(SUBCRIPTION_ON)

void loadSubcriptionPlan()
{
  EEPROMRead(SUBSCRIPTION_DATA_ADDR,(uint8_t*)&subscribeParam,sizeof(struct subscriptionParam_t));
  printSubscriptionData(&subscribeParam);
}

void saveSubscriptionPlan()
{
  EEPROMUpdate(SUBSCRIPTION_DATA_ADDR,(uint8_t*)&subscribeParam,sizeof(struct subscriptionParam_t));
}

void initiateDummySucbscriptionData()
{
  subscribeParam.header.id = RCMS_DEVICE_ID;
  subscribeParam.isDeviceHasSubscription = 1;
  subscribeParam.isSubscriptionOn = 0;
  subscribeParam.remainingDays = 55;
  printSubscriptionData(&subscribeParam);
  EEPROMUpdate(SUBSCRIPTION_DATA_ADDR, (uint8_t*)&subscribeParam,sizeof(struct subscriptionParam_t));
  loadSubcriptionPlan();
}

void printSubscriptionData(struct subscriptionParam_t *subParam)
{
  Serial.println("------- | Subscription | -------");
  Serial.print("is DeviceHasSubscription :"); Serial.println(subParam->isDeviceHasSubscription);
  Serial.print("is SubscriptionOn :"); Serial.println(subParam->isSubscriptionOn);
  Serial.print("remaining Days :"); Serial.println(subParam->remainingDays);
}
#endif

// void espWdtBegin()
// {
//   // pinMode(WDT_PIN, OUTPUT);
//   espWdtReset();
// }

// void espWdtReset(bool bResetFlag)
// {
//   static uint32_t millisEspReset = 0;
//   if (millis() - millisEspReset > 5000 || bResetFlag)
//   {
//     pinMode(WDT_PIN, OUTPUT);
//     digitalWrite(WDT_PIN,LOW);
//     delay(1); 
//     pinMode(WDT_PIN, INPUT);
//     millisEspReset = millis();
//     Serial.println("watchdog reset");
//   }
  
// }
#endif


