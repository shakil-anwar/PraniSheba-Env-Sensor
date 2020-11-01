#include "radio.h"


const byte address[6] = "00006";

void radio_begin()
{
  nrf_begin();
  nrf_common_begin();
  nrf_tx_begin();
}
