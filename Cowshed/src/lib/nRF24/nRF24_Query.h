#ifndef _NRF24_QUERY_H_
#define _NRF24_QUERY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "nRF24.h"

typedef struct addr_t
{
    uint8_t node[5];
    uint8_t type;
    uint8_t opcode;
    uint8_t checksum; 
}addr_t;


typedef uint8_t *(*queryResolver_t)(uint8_t *);
typedef void (*addrFun_t)(addr_t*);
typedef struct query_t
{
    uint8_t type;
    uint8_t opcode;
} query_t;

    /****************Common API**************************/
    void nrfQueryBufferSet(uint8_t *bufPtr, uint8_t len);
    /********Client side API*************/
    void nrfQueryClientSet(uint8_t queryPipe, uint8_t *queryTxAddr);
    uint8_t *nrfQuery(uint8_t type, uint8_t opcode,void *bufPtr, uint8_t len);
    void nrfTxAddrRestore(uint8_t lsByte);
    /********Server Side API**************/
    void nrfQueryServerSet(uint8_t pipe, uint8_t *addr, queryResolver_t queryResolver);
    void nrfQueryHandler(query_t query);
    void nrfQryServerEnd();

    /******************Generic API*************************/
    uint32_t nrfPing();

    void nrfSetAddrGenerator(uint8_t *(*getAddr)());
    void nrfServerAddrResolve(query_t qry);
    void nrfGetAddr(addr_t *addrPtr);

    bool nrfTxHasAddr(addr_t *addrPtr, addrFun_t read);
    void nrfTxAddrHandler(addrFun_t read, addrFun_t save);

    void nrfTxReady();
    void nrfTxAddrReset(addrFun_t save);

#ifdef __cplusplus
}
#endif

#endif
