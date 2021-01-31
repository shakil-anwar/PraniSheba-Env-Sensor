#include "nRF24.h"
#include "nRF24_Query.h"

// typedef enum nrf_main_state_t
// {
//     NRF_MAIN_ACQUIRE,
//     NRF_MAIN_SEND,
//     NRF_MAIN_WAIT,
//     NRF_MAIN_SUCCESS,
//     NRF_MAIN_FAILED,
//     NRF_MAIN_END
// } nrf_main_state_t;

// bool nrfAsyncSend(void *dataPtr, uint8_t packetSz, uint8_t packetLen)
// {
//     void *ptr = dataPtr;
//     do
//     {
//         switch (_nrfMainState)
//         {
//         case NRF_MAIN_ACQUIRE:

//             break;
//         case NRF_MAIN_SEND:
//             nrfWrite(ptr, packetSz);
//             break;
//         case NRF_MAIN_WAIT:
//             if (_nrfIrqState == NRF_SUCCESS)
//             {
//                 _nrfMainState = NRF_MAIN_SUCCESS;
//             }
//             else if (_nrfIrqState == NRF_FAIL)
//             {
//                 _nrfMainState = NRF_MAIN_FAILED;
//             }
//             break;
//         case NRF_MAIN_SUCCESS:
//             ptr += packetLen;

//             break;
//         case NRF_MAIN_FAILED:
//             break;
//         default:
//             break;
//         }
//         nrfWrite(ptr, packetSz);

//     } while ();

//     // void *ptr = dataPtr;
//     // switch (_nrfMainState)
//     // {
//     // case NRF_MAIN_ACQUIRE:

//     //     break;
//     // case NRF_MAIN_SEND:
//     //     nrfWrite(ptr, packetSz);
//     //     _nrfMainState = NRF_MAIN_WAIT;
//     // case NRF_MAIN_WAIT:
//     //     if (_nrfIrqState == NRF_SUCCESS)
//     //     {
//     //         _nrfMainState = NRF_MAIN_SUCCESS;
//     //     }
//     //     else if(_nrfIrqState == NRF_FAIL)
//     //     {
//     //         _nrfMainState = NRF_MAIN_FAILED;
//     //     }
//     //     break;
//     // case NRF_MAIN_SUCCESS:
//     //     ptr += packetLen;
//     //     break;
//     // case NRF_MAIN_FAILED:
//     // break;
//     // default:
//     //     break;
//     // }
// }