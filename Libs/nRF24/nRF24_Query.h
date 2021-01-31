#ifndef _NRF24_QUERY_H_
#define _NRF24_QUERY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "nRF24.h"

    typedef uint8_t *(*queryResolver_t)(uint8_t *);

    typedef enum nrf_main_state_t
    {
        NRF_MAIN_ACQUIRE,
        NRF_MAIN_SEND,
        NRF_MAIN_WAIT,
        NRF_MAIN_SUCCESS,
        NRF_MAIN_FAILED,
        NRF_MAIN_END
    } nrf_main_state_t;

    typedef struct query_t
    {
        uint8_t type;
        uint8_t opcode;
    } query_t;

    /****************Common API**************************/
    void nrfQueryBufferSet(uint8_t *bufPtr, uint8_t len);
    void nrfSendQuery(uint8_t *data, uint8_t len);

    /********Client side API*************/
    // void nrfQueryClientSet(uint8_t queryPipe, uint8_t *mainTxAddr, uint8_t *queryTxAddr);
    void nrfQueryClientSet(uint8_t queryPipe, uint8_t *queryTxAddr);
    uint8_t *nrfReadQuery();
    uint8_t *nrfQuery(uint8_t type, uint8_t opcode);

    void nrfTxAddrRestore(uint8_t lsByte);
    /********Server Side API**************/
    void nrfQueryServerSet(uint8_t pipe, uint8_t *addr, queryResolver_t queryResolver);
    void nrfQueryServerStart();
    bool nrfQueryServerHandle();

    extern nrf_main_state_t _nrfMainState;

#ifdef __cplusplus
}
#endif

#endif
