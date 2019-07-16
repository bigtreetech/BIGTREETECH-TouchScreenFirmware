#ifndef _SW_SPI_H_
#define _SW_SPI_H_

#include "stm32f10x_conf.h"

typedef enum
{
  _SPI_MODE0,
  _SPI_MODE1,
  _SPI_MODE2,
  _SPI_MODE3,
}_SPI_MODE;

typedef struct
{
  GPIO_TypeDef *cs_port;
  uint16_t      cs_pin;
  GPIO_TypeDef *sck_port;
  uint16_t      sck_pin; 
  GPIO_TypeDef *miso_port;
  uint16_t      miso_pin;
  GPIO_TypeDef *mosi_port;
  uint16_t      mosi_pin;
  _SPI_MODE     mode;
}_SW_SPI;


void SW_SPI_Config(_SW_SPI *sw_spi, _SPI_MODE mode,
  GPIO_TypeDef *cs_port,    uint16_t      cs_pin, 
  GPIO_TypeDef *sck_port,   uint16_t      sck_pin, 
  GPIO_TypeDef *miso_port,  uint16_t      miso_pin, 
  GPIO_TypeDef *mosi_port,  uint16_t      mosi_pin);

uint16_t SW_SPI_Read_Write(_SW_SPI *sw_spi, uint16_t d);
void SW_SPI_CS_Set(_SW_SPI *sw_spi, uint8_t level);

#endif
