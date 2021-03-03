#ifndef _SIMPLE_JSON_H_
#define _SIMPLE_JSON_H_
#include "Arduino.h"

char *enquote(char *dest, const  __FlashStringHelper  *source);
char *enquote(char *dest, const char *source);

char *add_int(char *dest, int value);
char *add_long(char *dest, long value);
char *add_ulong(char *dest, unsigned long value);
char *add_float(char *dest, float value,byte precision = 1);
char *add_str(char *dest, const char *source);

#endif 
