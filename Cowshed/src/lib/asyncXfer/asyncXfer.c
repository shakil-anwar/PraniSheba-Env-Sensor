#include "asyncXfer.h"
#include "Serial.h"

typedef enum state_e
{
    READ_MEM,
    TO_JSON,
    SERVER_SEND,
    WAIT_ACK,
    SEND_SUCCESS,
    FAILED,
} state_e;

// void printDebug(state_e state);

state_e sendState;

read_t _read;
send_t _send;
ackWait_t _ackWait;
millis_t _millis;

bool _isReady;
uint8_t *_ptr;
int _ackCode;
uint8_t _maxTryCount;
uint8_t _tryCount;
bool _debug;
uint32_t _startMillis;
uint8_t  _totalTime;

void xferBegin(read_t read, send_t send, ackWait_t ackWait, millis_t ms)
{
    _read = read;
    _send = send;
    _ackWait = ackWait;
    _millis = ms;
    // _tojson = NULL;

    _isReady = false;
    _ptr = NULL;
    _maxTryCount = 1; //default try once
    _debug = true;    //default Serial debug on
    SerialBegin(9600);
    SerialPrintlnF(P("asyncXfer setup done"));
}

void xferReady()
{
    _isReady = true;
    sendState = READ_MEM;
}

void xferSetMaxTry(uint8_t mxTry)
{
    _maxTryCount = mxTry;
}
void xferSetDebug(bool debug)
{
    _debug = debug;
}

bool xferSendLoop()
{

    if (_isReady)
    {
        switch (sendState)
        {
        case READ_MEM:
            _ptr = _read();
            if (_ptr != NULL)
            {
                if(_debug) 
                {
                    _startMillis = _millis();
                    SerialPrintlnF(P("Xfer : READ MEM : OK"));
                }
                sendState = SERVER_SEND;
                _tryCount = 0; //resets try count
            }
            break;
        case SERVER_SEND:
            if(_debug) {SerialPrintlnF(P("Xfer : SERVER_SEND"));}

            _send(_ptr);
            _tryCount++;
            sendState = WAIT_ACK;
            break;
        case WAIT_ACK:
            if(_debug) {SerialPrintlnF(P("Xfer : WAIT_ACK"));}
            _ackCode = _ackWait();
            if (_ackCode == 200)
            {

                sendState = SEND_SUCCESS;
                // _isReady = true;
            }
            else
            {
                sendState = FAILED;
                // _isReady = false;
            }

            break;
        case SEND_SUCCESS:
            if(_debug) 
            {
                SerialPrintF(P("Xfer : SEND_SUCCESS | Xfer Time : "));
                _totalTime = _millis() -  _startMillis;
                SerialPrintlnU32(_totalTime);
            }

            _isReady = true;
            sendState = READ_MEM;
            return _isReady;
            break;
        case FAILED:
            if(_tryCount < _maxTryCount)
            {
                if(_debug) {SerialPrintlnF(P("Xfer : FAILED | Resending"));}
                sendState = SERVER_SEND;
            }
            else
            {
                if(_debug) {SerialPrintlnF(P("Xfer : FAILED | Exiting xferSendLoop"));}
                _isReady = false;
            }
            
            break;
        }
    }
    return _isReady;
}


bool xferSendLoopV2()
{
	do
	{
        switch (sendState)
        {
            case READ_MEM:
                _ptr = _read();
                if (_ptr != NULL)
                {
                    if(_debug) 
                    {
                        _startMillis = _millis();
                        SerialPrintlnF(P("Xfer : READ MEM : OK"));
                    }
                    sendState = SERVER_SEND;
                    _tryCount = 0; //resets try count
                }
            break;
            case SERVER_SEND:
            if(_debug) {SerialPrintlnF(P("Xfer : SERVER_SEND"));}

            _send(_ptr);
            _tryCount++;
            sendState = WAIT_ACK;
            break;

            case WAIT_ACK:
            if(_debug) {SerialPrintlnF(P("Xfer : WAIT_ACK"));}
            _ackCode = _ackWait();
            if (_ackCode == 200)
            {

                sendState = SEND_SUCCESS;
                // _isReady = true;
            }
            else
            {
                sendState = FAILED;
                // _isReady = false;
            }

            break;
            case SEND_SUCCESS:
            if(_debug) 
            {
                SerialPrintF(P("Xfer : SEND_SUCCESS | Xfer Time : "));
                _totalTime = _millis() -  _startMillis;
                SerialPrintlnU32(_totalTime);
            }

            _isReady = true;
            sendState = READ_MEM;
            return _isReady;
            break;
            case FAILED:
            if(_tryCount < _maxTryCount)
            {
                if(_debug) {SerialPrintlnF(P("Xfer : FAILED | Resending"));}
                sendState = SERVER_SEND;
            }
            else
            {
                if(_debug) {SerialPrintlnF(P("Xfer : FAILED | Exiting xferSendLoop"));}
                _isReady = false;
            }
            
            break;

    	}

	}while(_ptr != NULL && _isReady);
    return _isReady;
}



bool xferSendLoopV3()
{
    uint16_t totalXfer = 0;
    do
    {
        _ptr = _read();
        if (_ptr != NULL)
        {
            if(_debug) 
            {
                _startMillis = _millis();
            }
            _send(_ptr);
            _ackCode = _ackWait();
            if(_ackCode == 200)
            {
                _isReady = true;
                totalXfer++;
                if(_debug)
                {
                     _totalTime = _millis() -  _startMillis;
                    SerialPrintF(P("Xfer:Success|Time:"));
                    SerialPrintlnU32(_totalTime);   
                } 
            }
            else
            {
                if(_debug) { SerialPrintF(P("Xfer:Failed"));}
                _isReady = false;
            }
        }

    }while(_ptr != NULL && _isReady);
    
    if(totalXfer>0)
    {
        SerialPrintF(P("----->Total Xfer Packet : "));
        SerialPrintlnU16(totalXfer);
    }
    return _isReady;
}

