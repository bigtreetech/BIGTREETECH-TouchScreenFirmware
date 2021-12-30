#ifndef _SW_SPI_H_
#define _SW_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "variants.h"  // for u8 etc...

typedef enum
{
  _SPI_MODE0,
  _SPI_MODE1,
  _SPI_MODE2,
  _SPI_MODE3,
} _SPI_MODE;

typedef struct
{
  uint16_t  cs;
  uint16_t  sck;
  uint16_t  miso;
  uint16_t  mosi;
  _SPI_MODE mode;
  uint8_t   dataSize;
} _SW_SPI;

void SW_SPI_Config(_SW_SPI *sw_spi, _SPI_MODE mode, u8 dataSize,
  uint16_t cs,
  uint16_t sck,
  uint16_t miso,
  uint16_t mosi);

uint16_t SW_SPI_Read_Write(_SW_SPI *sw_spi, uint16_t d);
void SW_SPI_CS_Set(_SW_SPI *sw_spi, uint8_t level);

#ifdef __cplusplus
}
#endif

#endif
