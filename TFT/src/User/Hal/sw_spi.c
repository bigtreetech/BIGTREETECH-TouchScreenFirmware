#include "sw_spi.h"

void GPIO_RCC_Set(GPIO_TypeDef *port)
{
  if(port == GPIOA)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  }
  else if(port == GPIOB)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  }
  else if(port == GPIOC)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  }
  else if(port == GPIOD)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  }
  else if(port == GPIOE)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
  }
  else if(port == GPIOF)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
  }
  else if(port == GPIOG)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
  }
}


void SW_SPI_Config(_SW_SPI *sw_spi, _SPI_MODE mode,
GPIO_TypeDef *cs_port,    uint16_t  cs_pin, 
GPIO_TypeDef *sck_port,   uint16_t  sck_pin, 
GPIO_TypeDef *miso_port,  uint16_t  miso_pin, 
GPIO_TypeDef *mosi_port,  uint16_t  mosi_pin)
{
//    uint32_t RCC_Define[_SPI_CNT] = {RCC_APB2Periph_GPIOA, 
//                                     RCC_APB2Periph_GPIOB, 
//                                     RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB};
  GPIO_InitTypeDef GPIO_InitStructure;
  
  sw_spi->cs_port   = cs_port;
  sw_spi->cs_pin    = cs_pin;
  sw_spi->sck_port  = sck_port;
  sw_spi->sck_pin   = sck_pin;
  sw_spi->miso_port = miso_port;
  sw_spi->miso_pin  = miso_pin;
  sw_spi->mosi_port = mosi_port;
  sw_spi->mosi_pin  = mosi_pin;
  sw_spi->mode      = mode;
  
  GPIO_RCC_Set(sw_spi->cs_port);
  GPIO_RCC_Set(sw_spi->sck_port);
  GPIO_RCC_Set(sw_spi->miso_port);
  GPIO_RCC_Set(sw_spi->mosi_port);
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  
  if(sw_spi->mode == _SPI_MODE0 || sw_spi->mode == _SPI_MODE1)
    GPIO_ResetBits(sw_spi->sck_port,  sw_spi->sck_pin);
  else
    GPIO_SetBits(sw_spi->sck_port,  sw_spi->sck_pin);
  GPIO_InitStructure.GPIO_Pin  =  sw_spi->sck_pin;    //SCK
  GPIO_Init(sw_spi->sck_port, &GPIO_InitStructure);
      
  GPIO_InitStructure.GPIO_Pin  =  sw_spi->mosi_pin;  //MOSI
  GPIO_Init(sw_spi->mosi_port, &GPIO_InitStructure);
  GPIO_SetBits(sw_spi->mosi_port,  sw_spi->mosi_pin);
  
  GPIO_InitStructure.GPIO_Pin  = sw_spi->cs_pin;     //CS
  GPIO_Init(sw_spi->cs_port, &GPIO_InitStructure);
  GPIO_SetBits(sw_spi->cs_port,  sw_spi->cs_pin);    
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin  = sw_spi->miso_pin;   //MISO
  GPIO_Init(sw_spi->miso_port, &GPIO_InitStructure);
}

#define SCK_HIGH()      GPIO_SetBits(sw_spi->sck_port, sw_spi->sck_pin)
#define SCK_LOW()       GPIO_ResetBits(sw_spi->sck_port, sw_spi->sck_pin)
#define MOSI_WRITE(n)   do{if(n) GPIO_SetBits(sw_spi->mosi_port,  sw_spi->mosi_pin); else GPIO_ResetBits(sw_spi->mosi_port,  sw_spi->mosi_pin);}while(0)
#define MISO_READ()     GPIO_ReadInputDataBit(sw_spi->miso_port, sw_spi->miso_pin)

uint16_t SW_SPI_Read_Write(_SW_SPI *sw_spi, uint16_t d)
{
  uint8_t data_bits = 8;
  uint8_t i=0;
  uint16_t rcv=0;
  switch(sw_spi->mode)
  {
    case _SPI_MODE0:
      for(i=0; i<data_bits; i++)
      {
        rcv <<=1;
        rcv |= MISO_READ();
        SCK_HIGH();
        MOSI_WRITE(d&0x80);
        d <<= 1;
        SCK_LOW();
      }
      break;
    case _SPI_MODE1:
      for(i=0; i<data_bits; i++)
      {
        MOSI_WRITE(d&0x80);
        d <<= 1;
        SCK_HIGH();
        rcv <<=1;
        rcv |= MISO_READ();
        SCK_LOW();
      }
      break;
    case _SPI_MODE2:
      for(i=0; i<data_bits; i++)
      {
        rcv <<=1;
        rcv |= MISO_READ();
        SCK_LOW();
        MOSI_WRITE(d&0x80);
        d <<= 1;
        SCK_HIGH();
      }
      break;
    case _SPI_MODE3:
      for(i=0; i<data_bits; i++)
      {
        MOSI_WRITE(d&0x80);
        d <<= 1;
        SCK_LOW();
        rcv <<=1;
        rcv |= MISO_READ();
        SCK_HIGH();
      }
      break;
  }
  return rcv;
}

void SW_SPI_CS_Set(_SW_SPI *sw_spi, uint8_t level)
{
  if(level)
    GPIO_SetBits(sw_spi->cs_port, sw_spi->cs_pin);
  else
    GPIO_ResetBits(sw_spi->cs_port, sw_spi->cs_pin);
}
