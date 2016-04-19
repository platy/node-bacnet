#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "device.h"
#include "config.h"
#include "bacdef.h"
#include "apdu.h"
#include "datalink.h"
#include "handlers.h"
#include "errorhandlers.h"
#include "tsm.h"
#include "newclient.h"

/* BBMD variables */
static long bbmd_timetolive_seconds = 60000;
static long bbmd_port = 0xBAC0;
static long bbmd_address = 0;
static int bbmd_result = 0;

void init_service_handlers() {
    fprintf(stderr, "Initialise client handlers\n");
    Device_Init(NULL);
    /* set the handler for all the services we don't implement
       It is required to send the proper reject message... */
    apdu_set_unrecognized_service_handler_handler(handler_unrecognized_service);
    /* we must implement read property - it's required! */
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_PROPERTY, handler_read_property);

    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_I_AM, handler_i_am_add);

    apdu_set_confirmed_ack_handler(SERVICE_CONFIRMED_READ_PROPERTY, handler_read_property_ack);
    apdu_set_error_handler(SERVICE_CONFIRMED_READ_PROPERTY, handler_error);

    apdu_set_confirmed_simple_ack_handler(SERVICE_CONFIRMED_WRITE_PROPERTY, handler_write_property_ack);
    apdu_set_error_handler(SERVICE_CONFIRMED_WRITE_PROPERTY, handler_error);
    /* handle any errors coming back */
    apdu_set_abort_handler(handler_abort);
    apdu_set_reject_handler(handler_reject);
}

void init_device_service_handlers() {
    fprintf(stderr, "Initialise device handlers\n");
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_WHO_IS, handler_who_is);
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_WRITE_PROPERTY, handler_write_property);
}

/** Register as a Foreign Device with the designated BBMD.
 * @ingroup DataLink
 * The BBMD's address, port, and lease time must be provided by
 * internal variables or Environment variables.
 * If no address for the BBMD is provided, no BBMD registration will occur.
 *
 * The Environment Variables depend on define of BACDL_BIP:
 *     - BACNET_BBMD_PORT - 0..65534, defaults to 47808
 *     - BACNET_BBMD_TIMETOLIVE - 0..65535 seconds, defaults to 60000
 *     - BACNET_BBMD_ADDRESS - dotted IPv4 address
 * @return Positive number (of bytes sent) on success,
 *         0 if no registration request is sent, or
 *         -1 if registration fails.
 */
int dlenv_register_as_foreign_device(struct BACNET_CONFIGURATION* config) {
    int retval = 0;
    if (config->bbmd_port) {
        bbmd_port = config->bbmd_port;
        if (bbmd_port > 0xFFFF) {
            bbmd_port = 0xBAC0;
        }
    }
    if (config->bbmd_ttl) {
        bbmd_timetolive_seconds = config->bbmd_ttl;
        if (bbmd_timetolive_seconds > 0xFFFF) {
            bbmd_timetolive_seconds = 0xFFFF;
        }
    }
    if (config->bbmd_address) {
        bbmd_address = bip_getaddrbyname(config->bbmd_address);
    }
    if (bbmd_address) {
        struct in_addr addr;
        addr.s_addr = bbmd_address;
        fprintf(stderr, "Registering with BBMD at %s:%ld for %ld seconds\n",
            inet_ntoa(addr), bbmd_port, bbmd_timetolive_seconds);
        retval =
            bvlc_register_with_bbmd(bbmd_address, htons((uint16_t) bbmd_port),
            (uint16_t) bbmd_timetolive_seconds);
        if (retval < 0)
            fprintf(stderr, "FAILED to Register with BBMD at %s \n",
                inet_ntoa(addr));
    }

    bbmd_result = retval;
    return retval;
}

/** Initialize the DataLink configuration from the config struct,
 * or else to defaults.
 * @ingroup DataLink
 */
const char * init_bacnet(struct BACNET_CONFIGURATION *config) {
    if (config->device_instance_id) {
        Device_Set_Object_Instance_Number(config->device_instance_id);
    }
    if (config->ip_port) {
        bip_set_port(htons(config->ip_port));
    } else {
        /* BIP_Port is statically initialized to 0xBAC0,
         * so if it is different, then it was programmatically altered,
         * and we shouldn't just stomp on it here.
         * Unless it is set below 1024, since:
         * "The range for well-known ports managed by the IANA is 0-1023."
         */
        if (ntohs(bip_get_port()) < 1024)
            bip_set_port(htons(0xBAC0));
    }
    if (config->apdu_timeout) {
        apdu_timeout_set(config->apdu_timeout);
        fprintf(stderr, "BACNET_APDU_TIMEOUT=%d\r\n", config->apdu_timeout);
    }
    if (config->apdu_retries) {
        apdu_retries_set(config->apdu_retries);
    }
    if (*config->iface) {
        if (!datalink_init(config->iface)) {
            fprintf(stderr, "Error %d %s \n", errno, strerror(errno)); // TODO return error code
            return "Failed to initialize data link on specified interface";
        }
    } else {
        if (!datalink_init(0)) {
            fprintf(stderr, "Error %d %s \n", errno, strerror(errno));
            return "Failed to initialize data link without interface";
        }
    }
    if (config->invoke_id) {
        tsm_invokeID_set(config->invoke_id);
    }
    dlenv_register_as_foreign_device(config);
    atexit(datalink_cleanup);
    return 0;
}
