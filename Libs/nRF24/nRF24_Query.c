#include "nRF24_Query.h"

#define QUERY_TIMEOUT_MS 1000

/**********************Global Vars*************************/  
// uint8_t *_mainTxAddr;
uint8_t *_queryAddr;
uint8_t _queryPipe;

queryResolver_t _queryResolver = NULL;
nrf_main_state_t _nrfMainState;

uint8_t *_queryBufPtr;
uint8_t _queryLen;

uint32_t _prevMillis;



/***********************************************************/
//Send data
//wait for send success
//change nrfIrq for rx mode
//nrf received mode
//data recceived

/**********************Common function***************************************/
void nrfQueryBufferSet(uint8_t *bufPtr, uint8_t len)
{
    _queryBufPtr = bufPtr;
    _queryLen = len;
}

void nrfSendQuery(uint8_t *data, uint8_t len)
{
    nrfSetTx(_queryAddr, true);
    nrfTXStart();
    // delay_us(130);
    nrfWrite(data, len); //NRF_WAIT
    nrfStartTransmit();
}
/**************************NRF Query client side functions*******************/

void nrfQueryClientSet(uint8_t queryPipe, uint8_t *queryTxAddr)
{
    // _mainTxAddr = mainTxAddr; //TX main address
    _queryAddr = queryTxAddr; //Address uses for query
    _queryPipe = queryPipe;       //pipe used for receiving query
}

void nrfTxAddrRestore(uint8_t lsByte)
{
    uint8_t addr[5];
    addr[0] = lsByte;

    memcpy(&addr[1],&_queryAddr[1],4);
    nrfSetTx(addr, true);

    // uint8_t readAddr[5];
    // read_bytes_in_register(RF24_TX_ADDR,readAddr,5);
    
    // //serial_print("\r\n Addr : ");
    // uint8_t i = 0; 
    // for(i = 0; i<5; i++)
    // {
    //     //serial_print_uint8(addr[i]);
    //     //serial_print("  ");
    // }
    // //serial_print("\r\n");
    // nrfTXStart();
}
// void nrfSendQuery(uint8_t *cmd, uint8_t len)


uint8_t *nrfReadQuery()
{
    switch (_nrfIrqState)
    {
    case NRF_SUCCESS:
        // nrfSetTx(_mainTxAddr,true);//After query send change tx address to previous tx addr.

        nrfSetPipe(_queryPipe,_queryAddr, true); //this will change pipe 5 address. 
        nrfRxStart();
        if(_nrfDebug){SerialPrintlnF(P("NRF Query Accepted"));}
        _nrfIrqState = NRF_WAIT;
    break;
    case NRF_WAIT:
    // //serial_print("\r\n Q_WAIT");
        break;
    case NRF_RECEIVED:
        if(_nrfDebug){SerialPrintlnF(P("NRF Query Received"));}
        nrfRead(_queryBufPtr,_queryLen);
        nrf_flush_rx();
        return _queryBufPtr;
    break;
    }
    return NULL;
}


uint8_t *nrfQuery(uint8_t type, uint8_t opcode)
{ 
    query_t query;
    query.type = type;
    query.opcode = opcode;

    nrfSendQuery((uint8_t*)&query,sizeof(query_t));
    
    uint32_t currentMillis = _Millis();
    uint32_t prevMillis =currentMillis;
    uint8_t *queryPtr = NULL;
    do
    {
        queryPtr = nrfReadQuery();
        if(queryPtr !=NULL)
        {
            break;
        }
        currentMillis = _Millis();
    } while ((currentMillis - prevMillis) < QUERY_TIMEOUT_MS);
    //TX address is changed in nrfReadQuery. Now just start as tx.
    // nrfTXStart();
    return queryPtr;
}

/**********************NRF Server Side functions**************************/

void nrfQueryServerSet(uint8_t pipe, uint8_t *addr, queryResolver_t queryResolver )
{
    _queryAddr = addr;
    _queryPipe = pipe;
    _queryResolver = queryResolver;
    nrfSetPipe(_queryPipe,_queryAddr, true);

}


void nrfQueryServerStart()
{
    _nrfMainState = NRF_MAIN_SEND;
}

bool nrfQueryServerHandle()
{
    // if(_nrfDebug){SerialPrintlnF(P("Entered nrfQueryServerHandle"));}
    switch(_nrfMainState)
    {
        case NRF_MAIN_SEND:
            if(_nrfDebug){SerialPrintlnF(P("NRF Resolving Query"));}
            if(_queryResolver)
            {
                _queryResolver(_queryBufPtr);
                isTXActive = true;
                nrfSendQuery(_queryBufPtr, _queryLen);
            }
            else
            {
                if(_nrfDebug){SerialPrintlnF(P("NRF Resolver Not Assigned"));}
            }

            _prevMillis = _Millis();
            _nrfMainState = NRF_MAIN_WAIT;
        break;
        case NRF_MAIN_WAIT:
            if((_Millis() - _prevMillis) <  QUERY_TIMEOUT_MS)
            {
                if(_nrfIrqState == NRF_SUCCESS)
                {
                   _nrfMainState = NRF_MAIN_END;
                }
                else if(_nrfIrqState == NRF_FAIL)
                {
                    if(_nrfDebug){SerialPrintlnF(P("NRF Query MAX_RT"));}
                    _nrfMainState = NRF_MAIN_END;
                }
            }
            else
            {
                if(_nrfDebug){SerialPrintlnF(P("NRF Query Rcv Failed"));}
                _nrfMainState = NRF_MAIN_END;
            }
        break;
        case NRF_MAIN_END:
            if(_nrfDebug){SerialPrintlnF(P("NRF Query End"));}
            nrf_ce_low(); //nrf goes TX to Standby-1

            // memset(_queryBufPtr,0, _queryLen);
            nrfRxStart(); //master goes to RX mode again
            isTXActive = false;
            return true;
        break;
        default:
            return false;
    }
    return false;
}

