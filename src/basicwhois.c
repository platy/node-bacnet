#include "basicwhois.h"

#include <stdio.h>
#include "address.h"
#include "datalink.h"
#include "txbuf.h"
#include "whois.h"


/** Send a Who-Is request to a remote network for a specific device, a range,
 * or any device.
 * If low_limit and high_limit both are -1, then the range is unlimited.
 * If low_limit and high_limit have the same non-negative value, then only
 * that device will respond.
 * Otherwise, low_limit must be less than high_limit.
 * @param target_address [in] BACnet address of target router
 * @param low_limit [in] Device Instance Low Range, 0 - 4,194,303 or -1
 * @param high_limit [in] Device Instance High Range, 0 - 4,194,303 or -1
 */
void Send_WhoIs_To_Network(
    BACNET_ADDRESS * target_address,
    int32_t low_limit,
    int32_t high_limit)
{
    int len = 0;
    int pdu_len = 0;
    int bytes_sent = 0;
    BACNET_NPDU_DATA npdu_data;
    BACNET_ADDRESS my_address;

    datalink_get_my_address(&my_address);
    /* encode the NPDU portion of the packet */
    npdu_encode_npdu_data(&npdu_data, false, MESSAGE_PRIORITY_NORMAL);

    pdu_len =
        npdu_encode_pdu(&Handler_Transmit_Buffer[0], target_address,
        &my_address, &npdu_data);
    /* encode the APDU portion of the packet */
    len =
        whois_encode_apdu(&Handler_Transmit_Buffer[pdu_len], low_limit,
        high_limit);
    pdu_len += len;
    bytes_sent =
        datalink_send_pdu(target_address, &npdu_data,
        &Handler_Transmit_Buffer[0], pdu_len);
#if PRINT_ENABLED
    if (bytes_sent <= 0)
        fprintf(stderr, "Failed to Send Who-Is Request (%s)!\n",
            strerror(errno));
#endif
}

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
