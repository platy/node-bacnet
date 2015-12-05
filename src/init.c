#include "init.h"

#include "datalink.h"
#include "dlenv.h"


void init_bacnet() {
  /* setup my info */
  Device_Set_Object_Instance_Number(BACNET_MAX_INSTANCE);
  dlenv_init();
  atexit(datalink_cleanup);
}