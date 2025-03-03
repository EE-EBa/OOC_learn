#ifndef EIO_PIN_H
#define EIO_PIN_H

/* includes */
#include "stdbool.h"
#include "stm32g0xx_hal.h"

/* public define */
enum pin_mode {
  PIN_MODE_INPUT = 0,
  PIN_MODE_INPUT_PULLUP,
  PIN_MODE_INPUT_PULLDOWN,
  PIN_MODE_OUTPUT,
  PIN_MODE_OUTPUT_OD,

  PIN_MODE_MAX

};

/* public typedef */
struct eio_pin_data {
  GPIO_TypeDef *gpio_x;
  uint16_t pin;
}; /* design for driving hal/LL lib , this definition is related
                    to hardware.*/

/* public class */
struct eio_pin {
  /* private */
  bool status;
  enum pin_mode mode;
  struct eio_pin_data data;

  /* public */
  void (*init)(struct eio_pin *const me, const char *name, enum pin_mode mode);
  bool (*get_status)(struct eio_pin  *const me);
  void (*set_status)(struct eio_pin *const me, bool status);
};


void eio_pin_ctor(struct eio_pin *me) ;
#endif
