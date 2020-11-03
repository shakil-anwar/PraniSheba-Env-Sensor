#include "All_Global.h"
#include "time.h"
#include "dataSchema.h"
#include "radio.h"


uint8_t buff[256];

void updateDisplay();
//void sendPayload();


//Task task1(5, &sendPayload); //send payload triggers after 5 second interval
Task task2(5, &updateDisplay);
Scheduler scheduler;


void system_setup(void)
{
  Serial.begin(9600);
  dataSchemaBegin();
//  rtcBegin();
  led_begin();

  humSensorBegin();
  mqBegin();
  mqCalibrate();
  radio_begin();
  
//  scheduler.addTask(&task1);
  scheduler.addTask(&task2);
  timeBegin();
  toSetverBegin();
  
  scheduler.begin(&second);
  Serial.println("Setup Done");
  pinMode(FLASH_CS, OUTPUT);
//  FLASH_DIS();
  
}

void payloadStateMachine()
{
  memQ.saveLoop();
  dataSendStateMachine();
}




//void enable_flash_write()
//{
//  FLASH_EN();
//  SPI.transfer(0x06);
//  FLASH_DIS();
//}
//
//void print_buff()
//{
//  Serial.print(buff[0],BIN);
//  Serial.print(" ");
//  Serial.print(buff[1],BIN);
//  Serial.print(" ");
//  Serial.print(buff[2],BIN);
//  Serial.print(" ");
//  Serial.print(buff[3],BIN);
//  Serial.print(" ");
//  Serial.println(buff[4],BIN);
//}
//
//uint8_t is_busy(void){
//  uint8_t response= 0; 
//  FLASH_EN();
//  SPI.transfer(0x05);
//  SPI.transfer(&response,1);
//  FLASH_DIS();
//  return (response & 0x01);
//}
//
//void read_page(uint32_t addr)
//{
//  Serial.println("Reading Page");
//  addr = (addr << 8);
//  uint8_t *ptr = (uint8_t*)&addr;
//  FLASH_EN();
//  SPI.transfer(0x03);
//  SPI.transfer(ptr[2]); 
//  SPI.transfer(ptr[1]);
//  SPI.transfer(ptr[0]);
//  for (uint16_t i = 0; i < 256; i++)
//  {
//    buff[i] = SPI.transfer(0);
//  }
//  FLASH_DIS();
//  for (uint16_t i = 0; i < 256; i++)
//  {
//    if((i%16)==0)
//    {
//      Serial.println("");
//    }
//    Serial.print(buff[i], HEX);
//    Serial.print(" ");
//  }
//  Serial.println("");
//}
//
//void write_page(uint32_t addr, uint8_t *buf, uint8_t len)
//{
//  Serial.println("Writing Page");
//  addr = (addr << 8);
//  uint8_t *ptr = (uint8_t*)&addr;
//  enable_flash_write();
//  while(is_busy());
//  FLASH_EN();
//  SPI.transfer(0x02);
//  SPI.transfer(ptr[2]); 
//  SPI.transfer(ptr[1]);
//  SPI.transfer(ptr[0]);
//  for (uint16_t i = 0; i < len; i++)
//  {
//    SPI.transfer(buf[i]);
//  }
//  FLASH_DIS();
//}
//
//void erase_block(uint32_t page_addr)
//{
//  page_addr = (page_addr/256);
//  page_addr = (page_addr << 16);
//  Serial.print("Block Address : ");
//  Serial.println(page_addr, HEX);
//  uint8_t *block_addr = (uint8_t*)page_addr;
//  
//  memset(buff,'\0',5);
//  buff[0]=0x44;
//  FLASH_EN();
//  SPI.transfer(0x50);
//  FLASH_DIS();
//  enable_flash_write();
//  while(is_busy());
//  FLASH_EN();
//  SPI.transfer(0x11);   // reading status register 3
//  SPI.transfer(buff,1);
//  FLASH_DIS();
//  FLASH_EN();
//  SPI.transfer(0x15);
//  SPI.transfer(buff,1);
//  FLASH_DIS();
//  Serial.print("Status Register 3: ");
//  print_buff();
//
//  Serial.println("Unlocking Block");
//  enable_flash_write();
//  while(is_busy());
//  FLASH_EN();
//  // Global Block Unlock
//  SPI.transfer(0x39);
//  SPI.transfer(block_addr[2]); 
//  SPI.transfer(block_addr[1]);
//  SPI.transfer(block_addr[0]);
//  FLASH_DIS();
//
//  //check block unlocked or not
//  while(is_busy());
//  enable_flash_write();
//  while(is_busy());
//  FLASH_EN();
//  SPI.transfer(0x3D);
//  SPI.transfer(block_addr[2]); 
//  SPI.transfer(block_addr[1]);
//  SPI.transfer(block_addr[0]);
//  SPI.transfer(buff,1);
//  FLASH_DIS();
//  Serial.print(" Block lock check: ");
//  Serial.println(buff[0]);
//
//  // Erase Flash Memory
//  while(is_busy());
//  enable_flash_write();
//  while(is_busy());
//  FLASH_EN();
//  SPI.transfer(0xD8);
//  SPI.transfer(block_addr[2]); 
//  SPI.transfer(block_addr[1]);
//  SPI.transfer(block_addr[0]);
//  FLASH_DIS();
//  
//  delay(3000);
//  while(is_busy());
//  memset(buff,'\0',5);
//  buff[0]=0x40;
//  enable_flash_write();
//  while(is_busy());
//  FLASH_EN();
//  SPI.transfer(0x11);   // reading status register 3
//  SPI.transfer(buff,1);
//  FLASH_DIS();
//  FLASH_EN();
//  SPI.transfer(0x15);
//  SPI.transfer(buff,1);
//  FLASH_DIS();
//  Serial.print("Status Register 3: ");
//  print_buff();
//  
//}
//
//void test_flash(void){
//  memset(buff,'\0',5);
//  buff[0]=0x40;
//  enable_flash_write();
//  FLASH_EN();
//  SPI.transfer(0x11);   // reading status register 3
//  SPI.transfer(buff,1);
//  FLASH_DIS();
//  FLASH_EN();
//  SPI.transfer(0x15);
//  SPI.transfer(buff,1);
//  FLASH_DIS();
//  Serial.print("Status Register 3: ");
//  print_buff();
//  Serial.println(is_busy());
//  read_page(300);
//  uint8_t data[4]={0x10,0x20,0x30,0x00};
//  write_page(700,data,4);
//  read_page(700);
//  erase_block(700);
//  while(is_busy());
//  read_page(700);
//  delay(2000);
//}
