#ifndef _NRF24_QUERY_SERVER_H_
#define _NRF24_QUERY_SERVER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include"nRF24_Qry_Common.h"

    /*****************query Server Functions*******************/
    
bool nrfQueryBeginServer(volatile struct qryObj_t *qryObj);


void nrfTxSetModeServer(enum addrMode_t addrMode);

void nrfQueryHandler(query_t *query);
void nrfQryServerEnd(query_t *qry);

void nrfServerConfigResolver(query_t *qry);



#ifdef __cplusplus
}
#endif

#endif