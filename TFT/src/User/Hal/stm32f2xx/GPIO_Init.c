#include "GPIO_Init.h"


/*
* bit 0:1 gpio mode  00: Input (reset state)  01: General purpose output mode  10: Alternate function mode  11: Analog mode
* bit 2   gpio output type  0: Output push-pull (reset state) 1: Output open-drain
* bit 3:4 gpio output speed  00: Low speed 01: Medium speed 10: High speed 11: Very high speed
* bit 5:6 gpio pull-up/pull-down  00: No pull-up, pull-down 01: Pull-up 10: Pull-down 11: Reserved
* bit 7   reserved
*/
GPIO_TypeDef* GPIO_Port[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI};

void GPIO_InitSet(uint16_t io, GPIO_MODE mode, uint8_t AF)
{
  u16 port = GPIO_GET_PORT(io);
  u16 pin = GPIO_GET_PIN(io);

  RCC->AHB1ENR |= 1 << port;  //Enable GPIO Clock

  GPIO_Port[port]->MODER &= ~(3 << (pin*2));  //clear mode bits
  GPIO_Port[port]->MODER |= (GPIO_MODE_GET_MODE(mode)) << (pin*2);  //set mode bits
  GPIO_Port[port]->PUPDR &= ~(3 << (pin*2));  //clear pull bits
  GPIO_Port[port]->PUPDR |= (GPIO_MODE_GET_PULL(mode)) << (pin*2);  //set pull bits

  if ((GPIO_MODE_GET_MODE(mode) == 1) || (GPIO_MODE_GET_MODE(mode) == 2)) //output
  {
    GPIO_Port[port]->OTYPER &= ~(1 << pin);  //clear output type bit
    GPIO_Port[port]->OTYPER |= GPIO_MODE_GET_OTYPE(mode) << pin;  //set output type bit
    GPIO_Port[port]->OSPEEDR &= ~(3 << (pin*2));  //clear speed bits
    GPIO_Port[port]->OSPEEDR |= (GPIO_MODE_GET_OSPEED(mode)) << (pin*2);  //set speed bits
  }

  if(GPIO_MODE_GET_MODE(mode) == GPIO_MODE_AF)
  {
    GPIO_Port[port]->AFR[pin >> 0x03] &= ~(0xF << ((pin & 0x07) * 4));  //clear alternate function bits
    GPIO_Port[port]->AFR[pin >> 0x03] |= AF<< ((pin & 0x07) * 4);  //set alternate function bits
  }
}

void GPIO_SetLevel(uint16_t io, uint8_t level)
{
  u16 port = GPIO_GET_PORT(io);
  u16 pin = GPIO_GET_PIN(io);
  if(level)
    GPIO_Port[port]->BSRRL = 1 << pin;
  else
    GPIO_Port[port]->BSRRH = 1 << pin;
}

uint8_t GPIO_GetLevel(uint16_t io)
{
  u16 port = GPIO_GET_PORT(io);
  u16 pin = GPIO_GET_PIN(io);

  if ((GPIO_Port[port]->IDR & (1 << pin)) != 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
