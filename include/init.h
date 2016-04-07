
#ifndef BASIC_INIT_H
#define BASIC_INIT_H

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * The Configuration Variables are:
 *   - apdu_timeout - set this value in milliseconds to change
 *     the APDU timeout.  APDU Timeout is how much time a client
 *     waits for a response from a BACnet device.
 *   - iface - set this value to dotted IP address (Windows) of
 *     the interface (see ipconfig command on Windows) for which you
 *     want to bind.  On Linux, set this to the /dev interface
 *     (i.e. eth0, arc0).  Default is eth0 on Linux, and the default
 *     interface on Windows.  Hence, if there is only a single network
 *     interface on Windows, the applications will choose it, and this
 *     setting will not be needed.
 *   - ip_port - UDP/IP port number (0..65534) used for BACnet/IP
 *     communications.  Default is 47808 (0xBAC0).
 *   - bbmd_port - UDP/IP port number (0..65534) used for Foreign
 *       Device Registration.  Defaults to 47808 (0xBAC0).
 *   - bbmd_ttl - number of seconds used in Foreign Device
 *       Registration (0..65535). Defaults to 60000 seconds.
 *   - bbmd_address - dotted IPv4 address of the BBMD or Foreign
 *       Device Registrar.
 *   - apdu_retries - not documented
 *   - invoke_id - not documented
 */
struct BACNET_CONFIGURATION {
    uint32_t device_instance_id;
    uint16_t ip_port;
    uint16_t apdu_timeout;
    uint8_t apdu_retries;
    const char* iface;
    uint8_t invoke_id;
    long bbmd_port;
    long bbmd_ttl;
    const char* bbmd_address;
};

const char * init_bacnet(struct BACNET_CONFIGURATION *config);
void init_service_handlers();
void init_device_service_handlers();

#ifdef __cplusplus
}
#endif

#endif
