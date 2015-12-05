#include "init.h"

#include "datalink.h"
#include "dlenv.h"


void init_bacnet() {
  /* Initialise the datalink parameters, dlenv was taken from a demo and sets the datalink parameters from environment
   variables, or sensible defaults, the fields are statically stored in bvlc.c. I would like to optionally initialise
   the link params from the js api and to allow making more than one link, but this may involve rewriting bvlc.c and some
   other files */
  dlenv_init();
  atexit(datalink_cleanup);
}