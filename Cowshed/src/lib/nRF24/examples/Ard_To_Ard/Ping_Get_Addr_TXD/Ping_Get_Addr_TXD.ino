#include "nRF24.h"
#include "nRF24_Query.h"
#include "EEPROM.h"

//#define BOARD_GAS_SENSOR
#define BOARD_TEST

#if defined(BOARD_GAS_SENSOR)
#define NRF_CE_PORT   PORTB
#define NRF_CE_PIN    PB2
#define NRF_CSN_PORT  PORTB
#define NRF_CSN_PIN   PB1
#define NRF_IRQ_PIN   3
#define BUS_FLASH_PIN 8
#elif defined(BOARD_TEST)
#define NRF_CE_PORT   PORTB
#define NRF_CE_PIN    PB1
#define NRF_CSN_PORT  PORTB
#define NRF_CSN_PIN   PB2
#define NRF_IRQ_PIN   2
#define BUS_FLASH_PIN 6
#elif defined(_BOARD_BASE_STATION)

#else
#error "Define a supported board"
#endif

#define ROM_ADDR_FOR_TXD  10
#define PAYLOAD_LEN 32
#define QUERY_PIPE 0

void txIsr(void);
void rxIsr(void);
void maxRtIsr(void);
void readAddr(addr_t *addrPtr);
void saveAddr(addr_t *addrPtr);


typedef struct unixTime_t
{
  uint8_t type;
  uint8_t opCode;
  uint32_t utime;
};
typedef union queryData_t
{
  unixTime_t unixTime;
};


uint8_t payload[PAYLOAD_LEN];

uint8_t pipe0Addr[5] = {230,1,0,5,3};
uint8_t txLsByte;
uint8_t txAddr[5];
queryData_t queryBuffer;

uint32_t packetCounter = 0;

uint32_t waitPrevMillis = 0;

void setup()
{
  Serial.begin(250000);
  pinMode(BUS_FLASH_PIN, OUTPUT);
  digitalWrite(BUS_FLASH_PIN , HIGH);

  nrfSetTimers(millis,NULL);
  nrfSetPin(&NRF_CE_PORT, NRF_CE_PIN, &NRF_CSN_PORT, NRF_CSN_PIN);
  nrfBegin(SPEED_2MB, POWER_ZERO_DBM, 1000000);
  nrfSetIrqs(txIsr, rxIsr, maxRtIsr);
//    nrfSetTx(pipe0Addr, true);
  nrfQueryClientSet(QUERY_PIPE, pipe0Addr);
//  nrfQueryBufferSet((uint8_t*)&queryBuffer, sizeof(queryData_t));


  pinMode(NRF_IRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(NRF_IRQ_PIN), nrfIrq, FALLING );
  delay(2000);

  nrfPowerDown();
  nrfStandby1();
  nrfTXMode();
  Serial.println("TX Setup Done");
}

void loop()
{
//  getQry();
   nrfPing();
   nrfTxAddrHandler(readAddr,saveAddr);
//   nrfGetAddr(txAddr,sizeof(txAddr));
//   Serial.print(F("TX Addr : ")); 
//   nrfDebugBuffer(txAddr,sizeof(txAddr));
  //  generateData(payload);
  //  printBuffer(payload,32);
  //  bool ok = nrfAckSend(payload,32);
  //  Serial.print(F("NRF Send : "));Serial.println(ok);
  delay(5000);
}


void txIsr(void)
{
//  nrfClearTxDs();
}

void rxIsr(void)
{
//  nrfClearRxDr();
}

void maxRtIsr(void)
{
//  nrfClearMaxRt();
  nrf_flush_rx();
  nrf_flush_tx();

}

void generateData(uint8_t *buf)
{
  uint8_t *p = buf;
  for (uint8_t i = 0; i < PAYLOAD_LEN; i++)
  {
    p[i] = random(100, 250);
  }
}

void printBuffer(uint8_t *buf, uint8_t len)
{
  for (uint8_t i = 0; i < len; i++)
  {
    Serial.print(buf[i]); Serial.print(' ');
  }
  Serial.println();
}

void getQry()
{
  memset(&queryBuffer, 0, sizeof(queryData_t));
  nrfStandby1();
  nrfTXStart();
  unixTime_t *uPtr = (unixTime_t*)nrfQuery(0, 3,&queryBuffer,sizeof(queryBuffer));
  if (uPtr != NULL)
  {
    Serial.println(uPtr -> utime);
  }
  else
  {
    Serial.println(F("return null"));
  }
}

void saveAddr(addr_t *addrPtr)
{
  Serial.println(F("NRF EEPROM Saving.."));
  uint8_t *ptr = (uint8_t*)addrPtr;
  for(uint8_t i = 0; i<sizeof(addr_t);i++)
  {
    EEPROM.update(ROM_ADDR_FOR_TXD+i, *(ptr+i));
  }
  nrfDebugBuffer(ptr, sizeof(addr_t));
}


void readAddr(addr_t *addrPtr)
{
  Serial.println(F("NRF EEPROM Reading.."));
  uint8_t *ptr = (uint8_t*)addrPtr;
  for(uint8_t i = 0 ; i< sizeof(addr_t); i++)
  {
    *(ptr+i) = EEPROM.read(ROM_ADDR_FOR_TXD+i);
  }
  nrfDebugBuffer(ptr, sizeof(addr_t));
}
