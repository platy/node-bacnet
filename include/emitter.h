
#ifndef EMITTER_H
#define EMITTER_H

#include <stdint.h>
#include "bacaddr.h"
#include "bacapp.h"
#include "rp.h"


#ifdef __cplusplus
extern "C" {
#endif

void emit_iam(uint32_t device_id, unsigned max_apdu, int segmentation, uint16_t vendor_id, BACNET_ADDRESS * src);
void emit_read_property_ack(uint8_t invoke_id, BACNET_READ_PROPERTY_DATA * data);
void emit_generic_ack(const char * type, uint8_t invoke_id);
void emit_abort(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    uint8_t abort_reason,
    bool server);
void emit_reject(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    uint8_t reject_reason);
void emit_error(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    BACNET_ERROR_CLASS error_class,
    BACNET_ERROR_CODE error_code);

#ifdef __cplusplus
}
#endif

#endif
