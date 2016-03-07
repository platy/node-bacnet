#include <stdint.h>
#include <time.h>
#include <uv.h>
#include "datalink.h"
#include "handlers.h"

static bool Error_Detected = false;
/* buffer used for receive */
static uint8_t Rx_Buf[MAX_MPDU] = {};


void runListen(void *arg) {
  BACNET_ADDRESS src = {};  /* address where message came from */
  uint16_t pdu_len = 0;
  unsigned timeout = 100;     /* milliseconds */
  time_t total_seconds = 0;
  time_t elapsed_seconds = 0;
  time_t last_seconds = 0;
  time_t current_seconds = 0;
  time_t timeout_seconds = 0;
  for (;;) {
      /* returns 0 bytes on timeout */
      pdu_len = datalink_receive(&src, &Rx_Buf[0], MAX_MPDU, timeout);
      /* process */
      if (pdu_len) {
          npdu_handler(&src, &Rx_Buf[0], pdu_len);
      }
      if (Error_Detected)
          break;
      /* increment timer - exit if timed out */
      elapsed_seconds = current_seconds - last_seconds;
      if (elapsed_seconds) {
#if defined(BACDL_BIP) && BBMD_ENABLED
          bvlc_maintenance_timer(elapsed_seconds);
#endif
      }
      total_seconds += elapsed_seconds;
      if (total_seconds > timeout_seconds)
          break;
      /* keep track of time for next check */
      last_seconds = current_seconds;
  }
}

void listenLoop() {
  uv_thread_t listen_thread_id;
  uv_thread_create(&listen_thread_id, runListen, 0);
}
