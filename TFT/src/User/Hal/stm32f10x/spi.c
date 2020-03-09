#include "spi.h"
#include "GPIO_Init.h"

#ifndef SPI1_CS_PIN
  #define SPI1_CS_PIN  PA4
#endif

#ifndef SPI2_CS_PIN
  #define SPI2_CS_PIN  PB12
#endif

#ifndef SPI3_CS_PIN
  #define SPI3_CS_PIN  PA15
#endif

static SPI_TypeDef *spi[_SPI_CNT] = {
  SPI1,  //CS--PA4   SCK--PA5   MISO--PA6   MOSI--PA7
  SPI2,  //CS--PB12  SCK--PB13  MISO--PB14  MOSI--PB15
  SPI3}; //CS--PA15  SCK--PB3   MISO--PB4   MOSI--PB5

static const uint16_t spi_cs[_SPI_CNT] = {SPI1_CS_PIN, SPI2_CS_PIN, SPI3_CS_PIN};  //CS
static const uint16_t spi_sck[_SPI_CNT]  = {PA5,  PB13, PB3};  //SCK
static const uint16_t spi_miso[_SPI_CNT] = {PA6,  PB14, PB4};  //MISO
static const uint16_t spi_mosi[_SPI_CNT] = {PA7,  PB15, PB5};  //MOSI

void SPI_GPIO_Init(uint8_t port)
{
  GPIO_InitSet(spi_sck[port], MGPIO_MODE_AF_PP, 0);  //SCK
  GPIO_InitSet(spi_miso[port], MGPIO_MODE_AF_PP, 0);  //MISO
  GPIO_InitSet(spi_mosi[port], MGPIO_MODE_AF_PP, 0);  //MOSI
  GPIO_InitSet(spi_cs[port], MGPIO_MODE_OUT_PP, 0);  //CS
  GPIO_SetLevel(spi_cs[port], 1);
}

void SPI_GPIO_DeInit(uint8_t port)
{
  //set all of spi pins to input
  GPIO_InitSet(spi_sck[port], MGPIO_MODE_IPN, 0);  //SCK
  GPIO_InitSet(spi_miso[port], MGPIO_MODE_IPN, 0);  //MISO
  GPIO_InitSet(spi_mosi[port], MGPIO_MODE_IPN, 0);  //MOSI
  GPIO_InitSet(spi_cs[port], MGPIO_MODE_IPN, 0);  //CS
}

// Ӳ��SPIЭ���ʼ��?
// baudrate��ȡֵ��ΧΪ 0-7
// ��������Ϊ 2^(baudrate+1) ��Ƶ��2-256��Ƶ
void SPI_Protocol_Init(uint8_t port, uint8_t baudrate)
{
  SPI_InitTypeDef SPI_InitStructure;

  SPI_I2S_DeInit(spi[port]);  //reset SPI clock
  switch(port)
  {
    case _SPI1: RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE); break;
    case _SPI2: RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE); break;
    case _SPI3: RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE); break;
    default: break;
  }

  SPI_InitStructure.SPI_BaudRatePrescaler = baudrate<<3;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_FirstBit =SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_Init(spi[port], &SPI_InitStructure);

  SPI_Cmd(spi[port],ENABLE);
}

void SPI_Config(uint8_t port)
{
  SPI_GPIO_Init(port);
  SPI_Protocol_Init(port, 1);
}

void SPI_DeConfig(uint8_t port)
{
  SPI_GPIO_DeInit(port);
  SPI_I2S_DeInit(spi[port]);
}

uint16_t SPI_Read_Write(uint8_t port, uint16_t d)
{
  while(SPI_I2S_GetFlagStatus(spi[port], SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(spi[port], d);
  while(SPI_I2S_GetFlagStatus(spi[port], SPI_I2S_FLAG_RXNE) == RESET);
  return SPI_I2S_ReceiveData(spi[port]);
}

void SPI_CS_Set(uint8_t port, uint8_t level)
{
  GPIO_SetLevel(spi_cs[port], level);
}
