#include "bsp.h"
#include "stdio.h"
#include "eio_pin.h"
#include <stdbool.h>
/* private variables */
static struct eio_pin pin_led;

/* public functions */
extern void initialise_monitor_handles(void);
int main(void) {
  initialise_monitor_handles();

  bsp_init();
  printf("semihosting test ok!\n");
  eio_pin_ctor(&pin_led);
  pin_led.init(&pin_led, "C.08", PIN_MODE_OUTPUT);
  while (1) {
    if ((elab_time_ms() % 1000) < 500) {
      pin_led.set_status(&pin_led, true);
    } else {
      pin_led.set_status(&pin_led, false);
    }
  }
}
