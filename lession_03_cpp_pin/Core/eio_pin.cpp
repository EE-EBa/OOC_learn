/* includes */
#include "eio_pin.h"
#include "elab_assert.h"
#include "elab_common.h"
#include "elab_log.h"
#include "qassert.h"
#include "string.h"

ELAB_TAG("ETO_PIN");
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
static void _translate_pin_name(const char *name, eio_pin_data_t *data);

/* public functions */
void eio_pin_t::init(const char *name, enum pin_mode mode) {
  bool valid = _check_pin_name_valid(name);
  Q_ASSERT(valid);
  //  elab_assert(valid);
  _translate_pin_name(name, &this->data_);
  this->mode_ = mode;

  /* configure gpio pin */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (mode == PIN_MODE_INPUT) {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
  } else if (mode == PIN_MODE_INPUT_PULLUP) {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;

  } else if (mode == PIN_MODE_INPUT_PULLDOWN) {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;

  } else if (mode == PIN_MODE_OUTPUT) {
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

  } else if (mode == PIN_MODE_OUTPUT_OD) {
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
  }

  GPIO_InitStruct.Pin = data_.pin;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(data_.gpio_x, &GPIO_InitStruct);

  get_status();
}

void eio_pin_t::set_status(bool status) {

  //elab_assert(mode_ == PIN_MODE_OUTPUT || mode_ == PIN_MODE_OUTPUT_OD);
  Q_ASSERT(mode_ == PIN_MODE_OUTPUT || mode_ == PIN_MODE_OUTPUT_OD);
  if (status != status_) {
    HAL_GPIO_WritePin(data_.gpio_x, data_.pin,
                      status ? GPIO_PIN_SET : GPIO_PIN_RESET);
    get_status();
    //elab_assert(status_ == status);
    Q_ASSERT(status_ == status);
  }
}

bool eio_pin_t::get_status(void) {
  GPIO_PinState status = HAL_GPIO_ReadPin(data_.gpio_x, data_.pin);
  status_ = (status == GPIO_PIN_SET) ? true : false;
  return status_;
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

static void _translate_pin_name(const char *name, eio_pin_data_t *data) {
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
