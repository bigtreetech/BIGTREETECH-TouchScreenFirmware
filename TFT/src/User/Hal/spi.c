#include "spi.h"

#ifndef SPI1_CS_RCC
  #define SPI1_CS_RCC  RCC_APB2Periph_GPIOA
  #define SPI1_CS_PORT GPIOA
  #define SPI1_CS_PIN  GPIO_Pin_4
#endif

#ifndef SPI2_CS_RCC
#define SPI2_CS_RCC  RCC_APB2Periph_GPIOB
#define SPI2_CS_PORT GPIOB
#define SPI2_CS_PIN  GPIO_Pin_12
#endif

#ifndef SPI3_CS_RCC
#define SPI3_CS_RCC  RCC_APB2Periph_GPIOA
#define SPI3_CS_PORT GPIOA
#define SPI3_CS_PIN  GPIO_Pin_15
#endif


static SPI_TypeDef *spi[_SPI_CNT] = {SPI1,  //CS--PA4   SCK--PA5   MISO--PA6   MOSI--PA7
                                     SPI2,  //CS--PB12  SCK--PB13  MISO--PB14  MOSI--PB15
                                     SPI3}; //CS--PA15  SCK--PB3   MISO--PB4   MOSI--PB5

GPIO_TypeDef *CS_GPIO_Port[_SPI_CNT]   = {SPI1_CS_PORT,  SPI2_CS_PORT,  SPI3_CS_PORT};  //CS
uint16_t CS_GPIO_Pin[_SPI_CNT]         = {SPI1_CS_PIN,   SPI2_CS_PIN,   SPI3_CS_PIN};
    
void SPI_GPIO_Init(uint8_t port, u8 DeInit)
{
  uint32_t RCC_Define[_SPI_CNT] = {RCC_APB2Periph_GPIOA | SPI1_CS_RCC, 
                                   RCC_APB2Periph_GPIOB | SPI2_CS_RCC, 
                                   RCC_APB2Periph_GPIOB | SPI3_CS_RCC};
  GPIO_TypeDef *SCK_GPIO_Port[_SPI_CNT]  = {GPIOA,       GPIOB,       GPIOB};       //SCK
  uint16_t SCK_GPIO_Pin[_SPI_CNT]        = {GPIO_Pin_5,  GPIO_Pin_13, GPIO_Pin_3};
  GPIO_TypeDef *MISO_GPIO_Port[_SPI_CNT] = {GPIOA,       GPIOB,       GPIOB};       //MISO
  uint16_t MISO_GPIO_Pin[_SPI_CNT]       = {GPIO_Pin_6,  GPIO_Pin_14, GPIO_Pin_4};
  GPIO_TypeDef *MOSI_GPIO_Port[_SPI_CNT] = {GPIOA,       GPIOB,       GPIOB};       //MOSI
  uint16_t MOSI_GPIO_Pin[_SPI_CNT]       = {GPIO_Pin_7,  GPIO_Pin_15, GPIO_Pin_5};
  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_Define[port],ENABLE);
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_InitStructure.GPIO_Mode = DeInit ? GPIO_Mode_IN_FLOATING : GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin =  SCK_GPIO_Pin[port];   //SCK
  GPIO_Init(SCK_GPIO_Port[port],&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  MISO_GPIO_Pin[port];  //MISO
  GPIO_Init(MISO_GPIO_Port[port],&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  MOSI_GPIO_Pin[port];  //MOSI
  GPIO_Init(MOSI_GPIO_Port[port],&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = DeInit ? GPIO_Mode_IN_FLOATING : GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin =  CS_GPIO_Pin[port];    //CS
  GPIO_Init(CS_GPIO_Port[port],&GPIO_InitStructure);
  GPIO_SetBits(CS_GPIO_Port[port],CS_GPIO_Pin[port]);
}

// 硬件SPI协议初始化
// baudrate的取值范围为 0-7
// 传输速率为 2^(baudrate+1) 分频，2-256分频
void SPI_Protocol_Init(uint8_t port, uint8_t baudrate)
{
  SPI_InitTypeDef SPI_InitStructure;
 
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
  SPI_GPIO_Init(port, 0);
  SPI_Protocol_Init(port,SPI_BaudRatePrescaler_2);
}

void SPI_DeConfig(uint8_t port)
{
  SPI_GPIO_Init(port, 1);
  switch(port)
  {
    case _SPI1:
      RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);
      RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);
      break;
    case _SPI2:
      RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);
      RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);
      break;
    case _SPI3: 
      RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,ENABLE);
      RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,DISABLE);
      break;
    default: break;
  }  
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
  if(level)
    CS_GPIO_Port[port]->BSRR = CS_GPIO_Pin[port];
  else
    CS_GPIO_Port[port]->BRR = CS_GPIO_Pin[port];
}
