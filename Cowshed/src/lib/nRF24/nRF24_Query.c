#include "nRF24_Query.h"

#define QUERY_TIMEOUT_MS 1000
#define PING_TYPE     200
#define PING_OPCODE   201
#define ADDR_OPCODE   204

uint8_t *nrfReadQuery(uint8_t *ptr, uint8_t len);
void pong(query_t qry);
uint8_t checksum(void *buffer, uint8_t len);
/**********************Global Vars*************************/
uint8_t *_queryAddr;
uint8_t _queryPipe;

queryResolver_t _queryResolver = NULL;
uint8_t *_queryBufPtr;
uint8_t _queryLen;

uint8_t *(*_getAddr)() = NULL;

typedef struct ping_t
{
    uint32_t second;
    uint32_t ms;
    uint8_t type;
    uint8_t opcode;
    uint8_t padding;
    uint8_t checksum;  
}ping_t;


uint8_t _txAddr[5];

/**********************Common function***************************************/
void nrfQueryBufferSet(uint8_t *bufPtr, uint8_t len)
{
    _queryBufPtr = bufPtr;
    _queryLen = len;
}

/**************************NRF Query client side functions*******************/

void nrfQueryClientSet(uint8_t queryPipe, uint8_t *queryTxAddr)
{
    _queryAddr = queryTxAddr; //Address uses for query
    _queryPipe = queryPipe;   //pipe used for receiving query
}

void nrfTxAddrRestore(uint8_t lsByte)
{
    uint8_t addr[5];
    addr[0] = lsByte;

    memcpy(&addr[1], &_queryAddr[1], 4);
    nrfSetTx(addr, true);
}

uint8_t *nrfReadQuery(uint8_t *ptr, uint8_t len)
{
    switch (_nrfIrqState)
    {
    case NRF_SUCCESS:
        // nrfSetTx(_mainTxAddr,true);//After query send change tx address to previous tx addr.

        nrfSetPipe(_queryPipe, _queryAddr, true); //Change to rx mode
        nrfRxStart();
        #if defined(ARDUINO_ARCH_AVR)
        // if (_nrfDebug)
        // {
        //     SerialPrintlnF(P("NRF Query Accepted"));
        // }
        #endif
        _nrfIrqState = NRF_WAIT;
        break;
    case NRF_WAIT:
        // //serial_print("\r\n Q_WAIT");
        break;
    case NRF_RECEIVED:
        #if defined(ARDUINO_ARCH_AVR)
        // if (_nrfDebug)
        // {
        //     SerialPrintlnF(P("NRF Query Received"));
        // }
        #endif
        nrfRead(ptr, len);
        // nrf_flush_rx();
        return ptr;
        break;
    }
    return NULL;
}

uint8_t *nrfQuery(uint8_t type, uint8_t opcode,void *bufPtr, uint8_t len)
{
    query_t query;
    query.type = type;
    query.opcode = opcode;

    if(_nrfDebug){SerialPrintlnF(P("Querying.."));}
    // nrfSendQuery((uint8_t *)&query, sizeof(query_t));
    nrfToTxAndSend(_queryAddr,(uint8_t *)&query, sizeof(query_t));

    uint32_t currentMillis = _Millis();
    uint32_t prevMillis = currentMillis;
    uint8_t *queryPtr = NULL;
    do
    {
        queryPtr = nrfReadQuery((uint8_t*)bufPtr,len);
        if (queryPtr != NULL)
        {
            if(_nrfDebug){SerialPrintlnF(P("<Qry Ok>"));}
            break;
        }      
        currentMillis = _Millis();
    } while ((currentMillis - prevMillis) < QUERY_TIMEOUT_MS);

    if(queryPtr == NULL)
    {
        if(_nrfDebug){SerialPrintlnF(P("<Qry Failed>"));}
    }
    return queryPtr;
}


/**********************NRF Server Side functions**************************/

void nrfQueryServerSet(uint8_t pipe, uint8_t *addr, queryResolver_t queryResolver)
{
    _queryAddr = addr;
    _queryPipe = pipe;
    _queryResolver = queryResolver;
    nrfSetPipe(_queryPipe, _queryAddr, true);
}

void nrfQueryHandler(query_t query)
{
    if (_nrfDebug)
    {
        SerialPrintF(P("Qry type :")); SerialPrintU8(query.type);
        SerialPrintF(P(" | Qry opcode :")); SerialPrintlnU8(query.opcode);
    }
    
    if(query.type == PING_TYPE)
    {
     //Handle Internal query 
     switch(query.opcode)
     {
         case PING_OPCODE:
         pong(query);
         break;
         case ADDR_OPCODE:
         nrfServerAddrResolve(query);
         break;
     }
    }
    else
    {
      //handle user query
       if (_queryResolver)
        {
            _queryResolver(_queryBufPtr);
            //  nrfSendQuery(_queryBufPtr, _queryLen);
            nrfToTxAndSend(_queryAddr, _queryBufPtr, _queryLen);
        }
        else
        {
            if (_nrfDebug)
            {
                SerialPrintlnF(P("NRF Qry Resolver Not Assigned"));
            }
        }
    }

}


void nrfQryServerEnd()
{
    //call this function in txIrq and mxRtIrq
    nrfRxStart();
    if(_nrfIrqState == NRF_SUCCESS)
    {
        SerialPrintlnF(P("Qry Response Received"));
    }
    else
    {
        SerialPrintlnF(P("Qry Response Failed"));
    }
    
}



uint8_t checksum(void *buffer, uint8_t len)
{
  uint8_t *p = (uint8_t*)buffer;
  
  uint16_t sum = 0;
  uint8_t i;
  for(i = 0; i< len; i++)
  {
    sum += (uint16_t)p[i];
  }
  return (uint8_t)sum;
}

void pong(query_t qry)
{
    ping_t ping;
    if(_seconds != NULL)
    {
        ping.second = _seconds();
    }
    else
    {
        SerialPrintF(P("zero called"));
        ping.second = 0;
    }
    if(_Millis != NULL)
    {
        ping.ms = _Millis();
    }
    else
    {
       ping.ms = 0;
    }
    
    ping.type = qry.type;
    ping.opcode = qry.opcode;
    ping.padding = 0;
    ping.checksum = checksum(&ping, sizeof(ping_t)-1);
    
    SerialPrintF(P("second : "));SerialPrintlnU32(ping.second);
    SerialPrintF(P("ms : "));SerialPrintlnU32(ping.ms);
    SerialPrintF(P("type : "));SerialPrintlnU8(ping.type);
    SerialPrintF(P("opcode : "));SerialPrintlnU8(ping.opcode);
    SerialPrintF(P("Checksum : "));SerialPrintlnU8(ping.checksum);

    nrfToTxAndSend(_queryAddr, (const uint8_t *)&ping, sizeof(ping_t));
}

uint32_t nrfPing()
{
  ping_t ping;
  nrfStandby1();
  nrfTXStart();

  ping_t *pingPtr = nrfQuery(PING_TYPE,PING_OPCODE,&ping,sizeof(ping_t));
  if (pingPtr != NULL)
  {
    // SerialPrintF(P("second : "));SerialPrintlnU32(pingPtr -> second);
    // SerialPrintF(P("ms : "));SerialPrintlnU32(pingPtr -> ms);
    // SerialPrintF(P("type : "));SerialPrintlnU8(pingPtr -> type);
    // SerialPrintF(P("opcode : "));SerialPrintlnU8(pingPtr -> opcode);
    // SerialPrintF(P("Checksum : "));SerialPrintlnU8(pingPtr -> checksum);
    if(checksum(&ping, sizeof(ping_t)-1) != pingPtr -> checksum)
    {
        if(_nrfDebug)
        {
          SerialPrintlnF(P("Checksum mismatch"));
        }
    }   
    return pingPtr -> second;
  }
  else
  {
    if(_nrfDebug)
    {
        SerialPrintlnF(P("Ping Failed"));
    }
    return 0;
  } 
}

/*****************TX Address resolver functions****************/
void nrfSetAddrGenerator(uint8_t *(*getAddr)())
{
    _getAddr = getAddr;
}
void nrfServerAddrResolve(query_t qry)
{
    addr_t addr;

    // addr.node[0] = 1;
    // addr.node[1] = 2;
    // addr.node[2] = 3;
    // addr.node[3] = 4;
    // addr.node[4] = 5;
    uint8_t *addrPtr =  _getAddr(); //server generates adddress 
    memcpy(addr.node, addrPtr,sizeof(addr.node));

    addr.type = qry.type;
    addr.opcode = qry.opcode;
    addr.checksum = checksum(&addr, sizeof(addr_t)-1);
    
    SerialPrintF(P("type : "));SerialPrintlnU8(addr.type);
    SerialPrintF(P("opcode : "));SerialPrintlnU8(addr.opcode);
    SerialPrintF(P("Checksum : "));SerialPrintlnU8(addr.checksum);

    nrfToTxAndSend(_queryAddr, (const uint8_t *)&addr, sizeof(addr_t));
}

void nrfGetAddr(addr_t *addrPtr)
{
//   addr_t addr;
  nrfStandby1();
  nrfTXStart();

  addrPtr = nrfQuery(PING_TYPE,ADDR_OPCODE,addrPtr,sizeof(addr_t));
  if (addrPtr != NULL)
  {
    // SerialPrintF(P("Addr : "));
    // nrfDebugBuffer(addrPtr -> node, sizeof(addrPtr -> node));

    // SerialPrintF(P("type : "));SerialPrintlnU8(addrPtr -> type);
    // SerialPrintF(P("opcode : "));SerialPrintlnU8(addrPtr -> opcode);
    // SerialPrintF(P("Checksum : "));SerialPrintlnU8(addrPtr -> checksum);
    if(checksum(addrPtr, sizeof(addr_t)-1) != addrPtr -> checksum)
    {
        if(_nrfDebug)
        {
            SerialPrintlnF(P("Addr Qry checksum mismatch"));
        }
    }
  }
  else
  {
    if(_nrfDebug)
    {
        SerialPrintF(P("Addr Qry Failed"));
    }
  } 
}


bool nrfTxHasAddr(addr_t *addrPtr, addrFun_t read)
{
    read(addrPtr);
    if(addrPtr -> type == PING_TYPE && addrPtr -> opcode == ADDR_OPCODE)
    {
        // SerialPrintlnF(P("Device has address"));
        return true;
    }
    return false;
}

void nrfTxAddrHandler(addrFun_t read, addrFun_t save)
{
    //read addr strucute from memory
    addr_t addr;
    bool hasAddr = nrfTxHasAddr(&addr,read);
    if(hasAddr == false)
    {
        if(_nrfDebug){ SerialPrintlnF(P("Getting New Addr"));}
        nrfGetAddr(&addr);
        save(&addr);
    }
    //validate address using checksum 
    memcpy(_txAddr,addr.node,sizeof(addr.node));
    if(_nrfDebug)
    {
        SerialPrintF(P("TX Addr : ")); 
        nrfDebugBuffer(_txAddr,sizeof(_txAddr));
    }
}

void nrfTxAddrReset(addrFun_t save)
{
    if(_nrfDebug)
    {
        SerialPrintlnF(P("Resetting TX Addr"));
    }
    addr_t addr;
    save(&addr);
}

void nrfTxReady()
{
    nrfSetTx(_txAddr, true);
    nrfStandby1();
    nrfTXStart();
    if(_nrfDebug)
    {
        SerialPrintlnF(P("TX Ready for Transmit"));
    }     
}