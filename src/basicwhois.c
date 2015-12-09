#include "basicwhois.h"

#include <stdio.h>
#include "address.h"
#include "datalink.h"
#include "txbuf.h"
#include "client.h"


int whoisBroadcast(
        char* mac_ascii,
        long dnet,
        char* destination_mac_ascii,
        int32_t Target_Object_Instance_Min,
        int32_t Target_Object_Instance_Max) {
    BACNET_MAC_ADDRESS mac = { 0 };
    BACNET_MAC_ADDRESS adr = { 0 };
    BACNET_ADDRESS dest = { 0 };
    bool global_broadcast = true;

    if (mac_ascii != 0) {
        if (address_mac_from_ascii(&mac, mac_ascii)) {
            global_broadcast = false;
        }
    }
    if ((dnet >= 0) && (dnet <= BACNET_BROADCAST_NETWORK)) {
       global_broadcast = false;
    }
    if (destination_mac_ascii != 0) {
        if (address_mac_from_ascii(&adr, destination_mac_ascii)) {
            global_broadcast = false;
        }
    }

    if (global_broadcast) {
        datalink_get_broadcast_address(&dest);
    } else {
        if (adr.len && mac.len) {
            memcpy(&dest.mac[0], &mac.adr[0], mac.len);
            dest.mac_len = mac.len;
            memcpy(&dest.adr[0], &adr.adr[0], adr.len);
            dest.len = adr.len;
            if ((dnet >= 0) && (dnet <= BACNET_BROADCAST_NETWORK)) {
                dest.net = dnet;
            } else {
                dest.net = BACNET_BROADCAST_NETWORK;
            }
        } else if (mac.len) {
            memcpy(&dest.mac[0], &mac.adr[0], mac.len);
            dest.mac_len = mac.len;
            dest.len = 0;
            if ((dnet >= 0) && (dnet <= BACNET_BROADCAST_NETWORK)) {
                dest.net = dnet;
            } else {
                dest.net = 0;
            }
        } else {
            if ((dnet >= 0) && (dnet <= BACNET_BROADCAST_NETWORK)) {
                dest.net = dnet;
            } else {
                dest.net = BACNET_BROADCAST_NETWORK;
            }
            dest.mac_len = 0;
            dest.len = 0;
        }
    }

    if (Target_Object_Instance_Min > BACNET_MAX_INSTANCE) {
        fprintf(stderr, "device-instance-min=%u - it must be less than %u\n",
            Target_Object_Instance_Min, BACNET_MAX_INSTANCE + 1);
        return 1;
    }
    if (Target_Object_Instance_Max > BACNET_MAX_INSTANCE) {
        fprintf(stderr, "device-instance-max=%u - it must be less than %u\n",
            Target_Object_Instance_Max, BACNET_MAX_INSTANCE + 1);
        return 1;
    }
    /* send the request */
    Send_WhoIs_To_Network(&dest, Target_Object_Instance_Min,
        Target_Object_Instance_Max);
    return 0;
}
