#ifndef _SPI_ARD_WRAP_H
#define _SPI_ARD_WRAP_H
#if defined(ARDUINO_ARCH_AVR)
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SPIClass spi_type_t;

spi_type_t  *nrfSPI();

void spi_begin(uint32_t speed);
uint8_t spi_transfer(uint8_t reg);
uint16_t spi_transfer16(uint16_t input16);

#ifdef __cplusplus
}
#endif

#endif

#endif
