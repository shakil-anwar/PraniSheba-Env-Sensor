#include "spi_driver.h"
#if defined(ARDUINO_ARCH_AVR) || defined(ESP32_V010)
#include <SPI.h>

spi_type_t *spiPtr;
#if defined(GSM_V010) && defined(ESP32_DEV)
SPIClass  SPI_H(HSPI);
#endif

extern "C" {
  spi_type_t  *nrfSPI()
  {
#if defined(GSM_V010) && defined(ESP32_DEV)
    return &SPI_H;
#else
    return &SPI;
#endif
  }

  void spi_begin(uint32_t speed)
  {
    spiPtr = nrfSPI();
  #if defined(GSM_V010) && defined(ESP32_DEV)
    spiPtr -> begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);
    //  pinMode(HSPI_SS, OUTPUT); 
  #else
    spiPtr -> begin();
  #endif
    
 
  #if defined(ESP32_V010)
    spiPtr -> beginTransaction(SPISettings());
  #else
    spiPtr -> beginTransaction(SPISettings(speed, MSBFIRST, SPI_MODE0));
  #endif
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
