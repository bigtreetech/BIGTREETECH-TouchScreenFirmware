#include "spi.h"
#include "variants.h"  // for SPI3_SCK_PIN etc...
#include "GPIO_Init.h"

// SPI1 default pins config
#ifndef SPI1_SCK_PIN
  #define SPI1_SCK_PIN  PA5
#endif
#ifndef SPI1_MISO_PIN
  #define SPI1_MISO_PIN PA6
#endif
#ifndef SPI1_MOSI_PIN
  #define SPI1_MOSI_PIN PA7
#endif
// SPI2 default pins config
#ifndef SPI2_SCK_PIN
  #define SPI2_SCK_PIN  PB13
#endif
#ifndef SPI2_MISO_PIN
  #define SPI2_MISO_PIN PB14
#endif
#ifndef SPI2_MOSI_PIN
  #define SPI2_MOSI_PIN PB15
#endif
// SPI3 default pins config
#ifndef SPI3_SCK_PIN
  #define SPI3_SCK_PIN  PB3
#endif
#ifndef SPI3_MISO_PIN
  #define SPI3_MISO_PIN PB4
#endif
#ifndef SPI3_MOSI_PIN
  #define SPI3_MOSI_PIN PB5
#endif

static volatile uint32_t* const rcc_spi_rst[_SPI_CNT] = {
  &RCC->APB2RSTR,
  &RCC->APB1RSTR,
  &RCC->APB1RSTR,
};

static volatile uint32_t* const rcc_spi_en[_SPI_CNT] = {
  &RCC->APB2ENR,
  &RCC->APB1ENR,
  &RCC->APB1ENR,
};

static const uint32_t rcc_spi_bit[_SPI_CNT] = {
  0x00001000,  // RCC_APB2  bit 12
  0x00004000,  // RCC_APB1  bit 14
  0x00008000,  // RCC_APB1  bit 15
};

static SPI_TypeDef* const spi[_SPI_CNT] = {
  SPI1,  // SCK--PA5   MISO--PA6   MOSI--PA7
  SPI2,  // SCK--PB13  MISO--PB14  MOSI--PB15
  SPI3,  // SCK--PB3   MISO--PB4   MOSI--PB5
};

static const uint16_t spi_sck[_SPI_CNT]  = {SPI1_SCK_PIN,  SPI2_SCK_PIN,  SPI3_SCK_PIN};   // SCK
static const uint16_t spi_miso[_SPI_CNT] = {SPI1_MISO_PIN, SPI2_MISO_PIN, SPI3_MISO_PIN};  // MISO
static const uint16_t spi_mosi[_SPI_CNT] = {SPI1_MOSI_PIN, SPI2_MOSI_PIN, SPI3_MOSI_PIN};  // MOSI

void SPI_GPIO_Init(uint8_t port)
{
  GPIO_InitSet(spi_sck[port],  MGPIO_MODE_AF_PP, 0);  // SCK
  GPIO_InitSet(spi_miso[port], MGPIO_MODE_AF_PP, 0);  // MISO
  GPIO_InitSet(spi_mosi[port], MGPIO_MODE_AF_PP, 0);  // MOSI
}

void SPI_GPIO_DeInit(uint8_t port)
{
  // Set all of spi pins to input
  GPIO_InitSet(spi_sck[port],  MGPIO_MODE_IPN, 0);  // SCK
  GPIO_InitSet(spi_miso[port], MGPIO_MODE_IPN, 0);  // MISO
  GPIO_InitSet(spi_mosi[port], MGPIO_MODE_IPN, 0);  // MOSI
}

// port: SPI index
// baudrate:0-7, 2^(baudrate+1) : 2~256
void SPI_Protocol_Init(uint8_t port, uint8_t baudrate)
{
  *rcc_spi_rst[port] |= rcc_spi_bit[port];
  *rcc_spi_rst[port] &= ~rcc_spi_bit[port];  // Reset SPI clock

  *rcc_spi_en[port] |= rcc_spi_bit[port];  // Enable SPI clock

  spi[port]->CR1 = (0<<15)        // 0:2-line 1: 1-line
                 | (0<<14)        // in bidirectional mode 0:read only 1: read/write
                 | (0<<13)        // 0:disable CRC 1:enable CRC
                 | (0<<12)        // 0:Data phase (no CRC phase) 1:Next transfer is CRC (CRC phase)
                 | (0<<11)        // 0:8-bit data frame 1:16-bit data frame
                 | (0<<10)        // 0:Full duplex 1:Receive-only
                 | (1<<9)         // 0:enable NSS 1:disable NSS (Software slave management)
                 | (1<<8)         // This bit has an effect only when the SSM bit is set. The value of this bit is forced onto the NSS pin and the IO value of the NSS pin is ignored
                 | (0<<7)         // 0:MSB 1:LSB
                 | (1<<6)         // Enable SPI
                 | (baudrate<<3)  // bit3-5   000:fPCLK/2    001:fPCLK/4    010:fPCLK/8     011:fPCLK/16
                                  //          100:fPCLK/32   101:fPCLK/64   110:fPCLK/128   111:fPCLK/256
                 | (1<<2)         // 0:Slave 1:Master
                 | (1<<1)         // CPOL
                 | (1<<0);        // CPHA
}

void SPI_Config(uint8_t port)
{
  SPI_GPIO_Init(port);
  SPI_Protocol_Init(port, 1);
}

void SPI_DeConfig(uint8_t port)
{
  SPI_GPIO_DeInit(port);
  *rcc_spi_rst[port] |= rcc_spi_bit[port];
  *rcc_spi_rst[port] &= ~rcc_spi_bit[port];  // Reset SPI clock
}

uint16_t SPI_Read_Write(uint8_t port, uint16_t d)
{
  while ((spi[port]->SR & (1 << 1)) == RESET);  // wait for tx empty
  spi[port]->DR = d;
  while ((spi[port]->SR & (1 << 0)) == RESET);  // wait for rx no empty
  return spi[port]->DR;
}
