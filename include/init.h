
#ifndef BASIC_INIT_H
#define BASIC_INIT_H

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

void init_bacnet();
void init_service_handlers();
void init_device_service_handlers();

#ifdef __cplusplus
}
#endif

#endif
