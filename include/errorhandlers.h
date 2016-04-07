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

void handler_error(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    BACNET_ERROR_CLASS error_class,
    BACNET_ERROR_CODE error_code);
