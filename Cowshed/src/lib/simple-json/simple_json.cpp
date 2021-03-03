#include "simple_json.h"
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>


//This function enquote and concate string from flash memory. Saves RAM
char *enquote(char *dest, const  __FlashStringHelper  *source)
{
  uint8_t len = strlen_P((const char*)source);
  char *temp = dest;
  *temp = '\"';
  temp++;
  strncpy_P(temp, (const char*)source, len);
  temp = temp + len;
  *temp = '\"';
  temp++;
  *temp = ':';
  temp++;
  return temp;
}
//This function enquote string from RAM directly. so source string consumes RAM
char *enquote(char *dest, const char *source)
{
  uint8_t len = strlen(source);
  char *temp = dest;
  *temp = '\"';
  temp++;
  strncpy(temp, source, len);
  temp = temp + len;
  *temp = '\"';
  temp++;
  *temp = ':';
  temp++;
  return temp;
}

char *add_int(char *dest, int value)
{
  char *temp = dest;
  char buffer[12];
  itoa(value, buffer, 10);
  uint8_t len = strlen(buffer);
  strncpy(temp, buffer, len);
  temp = temp + len;
  *temp = ',';
  temp++;
  return temp;
}

char *add_long(char *dest, long value)
{
  char *temp = dest;
  char buffer[12];
  ltoa(value, buffer, 10);
  uint8_t len = strlen(buffer);
  strncpy(temp, buffer, len);
  temp = temp + len;
  *temp = ',';
  temp++;
  // Serial.print(F("long Diff: "));Serial.println(temp-dest);
  return temp;
}
char *add_ulong(char *dest, unsigned long value)
{
  char *temp = dest;
  char buffer[12];
  ultoa(value, buffer, 10);
  uint8_t len = strlen(buffer);
  strncpy(temp, buffer, len);
  temp = temp + len;
  *temp = ',';
  temp++;
  // Serial.print(F("long Diff: "));Serial.println(temp-dest);
  return temp;
}

char *add_float(char *dest, float value, byte precision)
{
  char *temp = dest;
  // Serial.print(F("Float in: "));Serial.println(value);
  
  // char buffer[12];
  // dtostrf(value,3,2,buffer);
  // uint8_t len = strlen(buffer);
  // Serial.print(F("Len: "));Serial.println(len);
  // strncat(temp, buffer, len);
  // // strcat(temp, buffer);
  // Serial.print(F("Float out: "));Serial.println(buffer);
  // temp = temp + len;
  // *temp = ',';
  // temp++;
  // Serial.print(F("Diff: "));Serial.println(temp-dest);

  char buf[10];
  dtostrf(value, 0, precision, buf);  //(var, min width, precision, buffer)
  byte len = strlen(buf);
  strncpy(temp,buf,len);
  temp = temp+len;
  *temp = ',';
  temp++;
  return temp;
}

char *add_str(char *dest, const char *source)
{
  uint8_t len = strlen(source);
  char *temp = dest;
  *temp = '\"';
  temp++;
  strncpy(temp, source, len);
  temp = temp + len;
  *temp = '\"';
  temp++;
  *temp= ',';
  temp++;
  return temp;
}

