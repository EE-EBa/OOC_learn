/* includes */
#include "bsp.h"
#include "eio_pin.h"
#include "eba_export.h"
#include "qassert.h"
#include "string.h"
#include <stdbool.h>


Q_DEFINE_THIS_FILE

/* private confi_guration */
#define LED_POLL_PERIOD_MS (100)

/* private variables */
struct eio_pin *led = NULL;

/* public functions */
void led_init(void) {
  led = eio_pin_find("LED1");
  Q_ASSERT(led != NULL);
  eio_pin_set_mode(led, PIN_MODE_OUTPUT_OD);
}
INIT_EXPORT(led_init, 2);

void led_poll(void) {
  if ((elab_time_ms() % 1000) < 500) {
    eio_pin_set_status(led, true);
  } else {
    eio_pin_set_status(led, false);
  }
}
POLL_EXPORT(led_poll, LED_POLL_PERIOD_MS);
