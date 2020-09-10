#include "radio.h"

//create an RF24 object
RF24 radio(10, 9);  // CE, CSN

const byte address[6] = "00006";

void radio_begin()
{
  radio.begin();
  //set the address
  radio.openWritingPipe(address);
  //Set module as transmitter
  radio.stopListening();
}

