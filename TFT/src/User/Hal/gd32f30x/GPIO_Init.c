#include "GPIO_Init.h"
/*
* bit 0:1 gpio mode  00: Input (reset state)  01: General purpose output mode  10: Alternate function mode  11: Analog mode
* bit 2   gpio output type  0: Output push-pull (reset state) 1: Output open-drain
* bit 3:4 gpio output speed  00: Low speed 01: Medium speed 10: High speed 11: Very high speed
* bit 5:6 gpio pull-up/pull-down  00: No pull-up, pull-down 01: Pull-up 10: Pull-down 11: Reserved
* bit 7   reserved
*/
const uint32_t GPIO_Port[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG};
const rcu_periph_enum RCU_Port[] = {RCU_GPIOA, RCU_GPIOB, RCU_GPIOC, RCU_GPIOD, RCU_GPIOE, RCU_GPIOF, RCU_GPIOG};


void GPIO_InitSet(uint16_t io, GPIO_MODE mode, uint32_t remap)
{
  uint16_t port = GPIO_GET_PORT(io);
  uint16_t pin = GPIO_GET_PIN(io);

  // RCU_REG_VAL(RCU_Port[port]) |= BIT(RCU_BIT_POS(RCU_Port[port]));        // Enable GPIO Clock
  rcu_periph_clock_enable(RCU_Port[port]);
  rcu_periph_clock_enable(RCU_AF);

  gpio_init(GPIO_Port[port], mode, GPIO_OSPEED_50MHZ, 1<<pin);

  if (remap)
    gpio_pin_remap_config(remap, ENABLE);
}

void GPIO_SetLevel(uint16_t io, uint8_t level)
{
  uint16_t port = GPIO_GET_PORT(io);
  uint16_t pin = GPIO_GET_PIN(io);

  if (level)
    GPIO_BOP(GPIO_Port[port]) = (uint32_t)1<<pin;
  else
    GPIO_BC(GPIO_Port[port]) = (uint32_t)1<<pin;
}

void GPIO_ToggleLevel(uint16_t io)
{
  uint16_t port = GPIO_GET_PORT(io);
  uint16_t pin = GPIO_GET_PIN(io);


  if((uint32_t)RESET !=(GPIO_OCTL(GPIO_Port[port])&(1<<pin)))
  {
    GPIO_BC(GPIO_Port[port]) = (uint32_t)1<<pin;
  }
  else
  {
    GPIO_BOP(GPIO_Port[port]) = (uint32_t)1<<pin;
  }

}

uint8_t GPIO_GetLevel(uint16_t io)
{
  uint16_t port = GPIO_GET_PORT(io);
  uint16_t pin = GPIO_GET_PIN(io);

  if((uint32_t)RESET != (GPIO_ISTAT(GPIO_Port[port])&(1<<pin)))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
