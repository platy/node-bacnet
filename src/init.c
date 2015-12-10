#include "init.h"

#include <stdio.h>
#include "datalink.h"
#include "dlenv.h"
#include "device.h"
#include "apdu.h"
#include "handlers.h"


void init_service_handlers(
    void)
{
    fprintf(stderr, "Initialise client handlers\n");
    Device_Init(NULL);
    /* set the handler for all the services we don't implement
       It is required to send the proper reject message... */
    apdu_set_unrecognized_service_handler_handler
        (handler_unrecognized_service);
    /* we must implement read property - it's required! */
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_PROPERTY,
        handler_read_property);
//    /* handle the reply (request) coming back */
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_I_AM, handler_i_am_add);
//    /* handle any errors coming back */
//    apdu_set_abort_handler(MyAbortHandler);
//    apdu_set_reject_handler(MyRejectHandler);
}

void init_device_service_handlers(
    void)
{
    fprintf(stderr, "Initialise device handlers\n");
    /* we need to handle who-is
       to support dynamic device binding to us */
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_WHO_IS, handler_who_is);
}

void init_bacnet() {
  /* Initialise the datalink parameters, dlenv was taken from a demo and sets the datalink parameters from environment
   variables, or sensible defaults, the fields are statically stored in bvlc.c. I would like to optionally initialise
   the link params from the js api and to allow making more than one link, but this may involve rewriting bvlc.c and some
   other files */
  dlenv_init();
  atexit(datalink_cleanup);
}