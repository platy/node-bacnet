#ifndef NEW_CLIENT_H
#define NEW_CLIENT_H

#include <bacaddr.h>
#include <bacapp.h>
#include <apdu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint8_t Send_Write_Property_Request_Address(
    BACNET_ADDRESS * dest,
    uint16_t max_apdu,
    BACNET_OBJECT_TYPE object_type,
    uint32_t object_instance,
    BACNET_PROPERTY_ID object_property,
    BACNET_APPLICATION_DATA_VALUE * object_value,
    uint8_t priority,
    uint32_t array_index);

void handler_write_property_ack(
    BACNET_ADDRESS * src,
    uint8_t invoke_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
