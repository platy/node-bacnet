#include "stdio.h"
#include "errorhandlers.h"
#include "address.h"
#include "bactext.h"
#include "emitter.h"


// TODO these currently just print the problems out, we'll want to send them back to the js, but also to correlate them
// TODO  with the requests using the invoke id and src

void handler_abort(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    uint8_t abort_reason,
    bool server)
{
    (void) server;
//    if (address_match(&Target_Address, src) &&
//        (invoke_id == Request_Invoke_ID)) {
        printf("BACnet Abort: %s\r\n",
            bactext_abort_reason_name((int) abort_reason));
//    }
}

void handler_reject(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    uint8_t reject_reason)
{
//    if (address_match(&Target_Address, src) &&
//        (invoke_id == Request_Invoke_ID)) {
        printf("BACnet Reject: %s\r\n",
            bactext_reject_reason_name((int) reject_reason));
//    }
}

void handler_error(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    uint8_t abort_reason,
    bool server)
{
    (void) server;
//    if (address_match(&Target_Address, src) &&
//        (invoke_id == Request_Invoke_ID)) {
        printf("BACnet Abort(%d): %s\n",
            invoke_id,
            bactext_abort_reason_name((int) abort_reason));
//    }
    emit_abort(src, invoke_id, abort_reason, server);
}
