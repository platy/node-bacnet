#include "stdint.h"
#include "address.h"

void handler_abort(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    uint8_t abort_reason,
    bool server);

void handler_reject(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    uint8_t reject_reason);
