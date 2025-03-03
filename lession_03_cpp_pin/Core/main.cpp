#include "bsp.h"
#include "eio_pin.h"
#include "elab_common.h"
#include "stdio.h"
#include "stdlib.h"
#include "qassert.h"
/* private variables */
static eio_pin_t pin_led;
Q_DEFINE_THIS_FILE

/* public functions */

extern "C"{
	extern void initialise_monitor_handles(void);
}


int main(void) {
  initialise_monitor_handles();

  bsp_init();
  printf("test test test!\n");
  pin_led.init("R.08", PIN_MODE_OUTPUT);
  while (1) {
    if ((elab_time_ms() % 1000) < 500) {
      pin_led.set_status(true);

    } else {
      pin_led.set_status(false);
    }
  }
}
