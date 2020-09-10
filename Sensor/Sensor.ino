#include "all_global.h"

void setup()
{
  Serial.begin(9600);
  system_setup();
//  led_begin();
}

void loop()
{
//  Serial.print(F("MQ4 | Rs: ")); Serial.print(mq4.Rs);
//  Serial.print(F(" | PPM: ")); Serial.println(mq4.getPPM());
  //Send message to receiver
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));

  Serial.print(F("Humidity: "));Serial.println(getSenorReading() -> hum);
  set_led_level(3, 3);
  delay(1000);

}
