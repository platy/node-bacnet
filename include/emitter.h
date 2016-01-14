
#ifndef EMITTER_H
#define EMITTER_H

#include <stdint.h>
#include "bacaddr.h"
#include "bacapp.h"


#ifdef __cplusplus
extern "C" {
#endif

void emit_iam(uint32_t device_id, unsigned max_apdu, int segmentation, uint16_t vendor_id, BACNET_ADDRESS * src);
void emit_read_property_ack(BACNET_READ_PROPERTY_DATA * data);

#ifdef __cplusplus
}
#endif

#endif
