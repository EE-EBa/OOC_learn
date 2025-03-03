/* includes */
#include "eio_pin.h"
#include "qassert.h"
#include "string.h"

Q_DEFINE_THIS_FILE

/* private variables */
static const GPIO_TypeDef *gpio_table[] = {
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
};

/* private functions prototype */
static bool _check_pin_name_valid(const char *name);
static void _translate_pin_name(const char *name, struct eio_pin_data *data);

/* public functions */

void eio_pin_set_status(struct eio_pin *const me, bool status) {
  Q_ASSERT(me != NULL);
  // elab_assert(mode_ == PIN_MODE_OUTPUT || mode_ == PIN_MODE_OUTPUT_OD);
  Q_ASSERT(me->mode == PIN_MODE_OUTPUT || me->mode == PIN_MODE_OUTPUT_OD);
  if (status != me->status) {
    HAL_GPIO_WritePin(me->data.gpio_x, me->data.pin,
                      status ? GPIO_PIN_SET : GPIO_PIN_RESET);
    me->get_status(me);
    // elab_assert(status_ == status);
    Q_ASSERT(me->status == status);
  }
}

bool eio_pin_get_status(struct eio_pin *const me) {
  GPIO_PinState status = HAL_GPIO_ReadPin(me->data.gpio_x, me->data.pin);
  me->status = (status == GPIO_PIN_SET) ? true : false;
  return me->status;
}

static bool _check_pin_name_valid(const char *name) {
  bool ret = true;
  uint8_t pin_number;

  if (!((strlen(name) == 4) && (name[1] == '.'))) {
    ret = false;
    goto exit;
  }

  if (((name[0] < 'A') || (name[0] > 'D')) ||
      ((name[2] < '0') || (name[2] > '1')) ||
      ((name[3] < '0') || (name[3] > '9'))) {
    ret = false;
    goto exit;
  }

  pin_number = (name[2] - '0') * 10 + (name[3] - '0');
  if (pin_number >= 16) {
    ret = false;
    goto exit;
  }

exit:
  return ret;
}

void eio_pin_init(struct eio_pin *me, const char *name, enum pin_mode mode) {
  Q_ASSERT(me != NULL);
  Q_ASSERT(name != NULL);

  bool valid = _check_pin_name_valid(name);
  Q_ASSERT(valid);

  _translate_pin_name(name, &me->data);
  me->mode = mode;

  /* configure GPIO pin. */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (mode == PIN_MODE_INPUT) {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
  } else if (mode == PIN_MODE_INPUT_PULLUP) {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
  } else if (mode == PIN_MODE_INPUT_PULLUP) {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  } else if (mode == PIN_MODE_OUTPUT) {
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
  } else if (mode == PIN_MODE_OUTPUT_OD) {
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
  }

  GPIO_InitStruct.Pin = me->data.pin;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(me->data.gpio_x, &GPIO_InitStruct);

  me->get_status(me);
}
void eio_pin_ctor(struct eio_pin *me) {
  me->init = eio_pin_init;
  me->get_status = eio_pin_get_status;
  me->set_status = eio_pin_set_status;
}

static void _translate_pin_name(const char *name, struct eio_pin_data *data) {
  Q_ASSERT(data != NULL);
  data->gpio_x = (GPIO_TypeDef *)gpio_table[name[0] - 'A'];
  data->pin = (1 << ((uint8_t)((name[2] - '0') * 10 + (name[3] - '0'))));

  if (name[0] == 'A') {
    __HAL_RCC_GPIOA_CLK_ENABLE();
  } else if (name[0] == 'B') {
    __HAL_RCC_GPIOB_CLK_ENABLE();
  } else if (name[0] == 'C') {
    __HAL_RCC_GPIOC_CLK_ENABLE();
  } else if (name[0] == 'D') {
    __HAL_RCC_GPIOD_CLK_ENABLE();
  }
}
