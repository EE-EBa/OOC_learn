#include "eio_pin.h"
#include "qassert.h"
#include "string.h"
#include <string.h>

Q_DEFINE_THIS_FILE
/* private variables */
static struct eio_pin *pin_list =
    NULL; /* pin_list is a head node of this link list */

/* private functions */

/* public functions */

void eio_pin_register(struct eio_pin *const me, const char *name,
                      struct eio_pin_ops const *ops, void *user_data) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(name != NULL);
  Q_ASSERT(ops != NULL);
  Q_ASSERT(user_data != NULL);

  me->name = name;
  me->user_data = user_data;
  me->ops = ops;
  me->next = pin_list;
  pin_list = me;

  me->ops->init(me);
  me->status = me->ops->get_status(me);
}

struct eio_pin *eio_pin_find(char const *name) {
  Q_ASSERT(name != NULL);
  Q_ASSERT(pin_list != NULL);
  struct eio_pin *pin = pin_list;
  while (pin != NULL) {
    if (strcmp(pin->name, name) == 0) {
      break;
    }
    pin = pin->next;
  }
  return pin;
}

void eio_pin_set_mode(struct eio_pin *const me, uint8_t mode) {
  Q_ASSERT(me != NULL);

  me->mode = mode;
  me->ops->set_mode(me, mode);
}

bool eio_pin_get_status(struct eio_pin *const me) {
  Q_ASSERT(me != NULL);

  me->status = me->ops->get_status(me);
  return me->status;
}

void eio_pin_set_status(struct eio_pin *const me, bool status) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(me->mode == PIN_MODE_OUTPUT_PP || me->mode == PIN_MODE_OUTPUT_OD);

  if (status != me->status) {
    me->ops->set_status(me, status);
    eio_pin_get_status(me);
    Q_ASSERT(me->status == status);
  }
}
