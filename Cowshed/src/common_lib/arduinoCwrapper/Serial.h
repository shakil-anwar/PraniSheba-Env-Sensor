#ifndef _SERIAL_ARD_H_
#define _SERIAL_ARD_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#if defined(ESP32_V010)
	#include <pgmspace.h>
#else
	#include <avr/pgmspace.h>
#endif

#define P(str) PSTR(str)

typedef struct HardwareSerial Serial_t;
Serial_t  *newMySerial();

void SerialBegin(uint32_t baud); // Hardware dependent

void SerialPrintF(const char *str); // Hardware dependent

void SerialPrintlnF(const char *str);
void SerialPrint(char *str);
void SerialPrintln(char *str);


void SerialPrintU8(uint8_t n);
void SerialPrintS8(int8_t n);
void SerialPrintU16(uint16_t n);
void SerialPrintS16(int16_t n);
void SerialPrintU32(uint32_t n);
void SerialPrintS32(int32_t n);


void SerialPrintlnU8(uint8_t n);
void SerialPrintlnS8(int8_t n);
void SerialPrintlnU16(uint16_t n);
void SerialPrintlnS16(int16_t n);
void SerialPrintlnU32(uint32_t n);
void SerialPrintlnS32(int32_t n);

void SerialPrintFloat(float n,uint8_t digit);
void SerialPrintlnFloat(float n,uint8_t digit);


#ifdef __cplusplus
}
#endif //End of __cplusplus

#endif //end of __SERIAL_H_
