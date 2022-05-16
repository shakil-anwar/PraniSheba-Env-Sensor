#include "./src/avr/avr.h"
#if defined(ESP32_V010) 
#include "./src/wifi/BS_WiFi.h"
#include <esp_task_wdt.h>

TaskHandle_t Task1AVR;   // avr code
TaskHandle_t Task2ESP;   // esp code
#endif




void setup()
{
#if defined(STAND_ALONE)
  wifiSetup();
#endif
  avrSetup();

// #if defined(ESP32_V010)
//   xTaskCreatePinnedToCore(
//                     Task1AVRcode,   /* Task function. */
//                     "Task1AVR",     /* name of task. */
//                     10000,       /* Stack size of task */
//                     NULL,        /* parameter of the task */
//                     1,           /* priority of the task */
//                     &Task1AVR,      /* Task handle to keep track of created task */
//                     1);          /* pin task to core 0 */                  
//   delay(500);

//   xTaskCreatePinnedToCore(
//                     Task2ESPcode, /* Function to implement the task */
//                     "Task2ESP", /* Name of the task */
//                     10000,  /* Stack size in words */
//                     NULL,  /* Task input parameter */
//                     2,  /* Priority of the task */
//                     &Task2ESP,  /* Task handle. */
//                     1); /* Core where the task should run */
// #endif


}

void loop()
{
  avrloop();
#if defined(STAND_ALONE)
  wifiLoop();
#endif
}



#if defined(ESP32_V010)
void Task1AVRcode( void * pvParameters ){
  // esp_task_wdt_init(30, false);
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  // avrSetup();

  for(;;){
    avrloop();
    esp_task_wdt_reset();
  }
}

void Task2ESPcode( void * parameter) {
  // esp_task_wdt_init(30, false);
  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());

  // wifiSetup();
  
  Serial.println("ESP is working");
  delay(5000);
  for(;;) {
    // Serial.println("Esp Working");
    wifiLoop();
    esp_task_wdt_reset();
  }
}
#endif
