#include "spi_driver.h"
#if defined(ARDUINO_ARCH_AVR)
#include <SPI.h>

spi_type_t *spiPtr;

extern "C" {
  spi_type_t  *nrfSPI()
  {
    return &SPI;
  }

  void spi_begin(uint32_t speed)
  {
    spiPtr = nrfSPI();
    spiPtr -> begin();
    spiPtr -> beginTransaction(SPISettings(speed, MSBFIRST, SPI_MODE0));
    spiPtr -> endTransaction();
  }
  uint8_t spi_transfer(uint8_t reg)
  {
    return spiPtr -> transfer(reg);
  }
  uint16_t spi_transfer16(uint16_t input16)
  {
    return spiPtr -> transfer16(input16);
  }
}

#endif
