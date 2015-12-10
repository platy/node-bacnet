
#ifndef EMITTER_H
#define EMITTER_H

#include <stdint.h>
#include "bacaddr.h"


#ifdef __cplusplus
extern "C" {
#endif

void emit_iam(uint32_t device_id, unsigned max_apdu, BACNET_ADDRESS * src);

#ifdef __cplusplus
}
#endif

#endif
