#include "SerialString.h"
#define LOOP_EXIT_AFTER_MIN 5
#define MIN_TO_MS 60000
#define LOOP_EXIT_AFTER_MS (LOOP_EXIT_AFTER_MIN*MIN_TO_MS)

uint8_t getSerialCmd()
{
  long timeout = LOOP_EXIT_AFTER_MS;
  while (!Serial.available() && --timeout)
  {
    delay(1);
  }
  // Serial.print(F("Loop breaks : ")); Serial.println(timeout);
  int cmd = Serial.parseInt();
  Serial.print(F("Input Cmd :")); Serial.println(cmd);
  return cmd;
}



int timedRead()
{
  int c;
  uint32_t last_millis = millis();
  do
  {
    c = Serial.read();
    if (c > 0 )
    {
      return c;
    }
  } while ((millis() - last_millis) < 1000);
  return -1;
}

char *stringRead(char *buffer)
{
  char *ptr  = buffer;

  int c = timedRead();
  while (c >= 0)
  {
    *ptr = (char)c;
    ptr++;
    c = timedRead();
  }
  *ptr = '\0'; //string terminator as extra safety
  return buffer;
}

char *getSerialString(char *str)
{
  long timeout = LOOP_EXIT_AFTER_MS;
  while (!Serial.available() && --timeout)
  {
    delay(1);
  }
  //  Serial.println(F("Serial Available"));
  return stringRead(str);
}
