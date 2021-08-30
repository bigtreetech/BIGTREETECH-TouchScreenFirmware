#include "Serial.h"
#include "includes.h"  // for infoHost

// dma rx buffer
DMA_CIRCULAR_BUFFER dmaL1Data[_UART_CNT] = {0};

// Config for USART Channel
typedef struct
{
  USART_TypeDef *uart;
  uint32_t dma_rcc;
  DMA_Channel_TypeDef *dma_chanel;
} SERIAL_CFG;

static const SERIAL_CFG Serial[_UART_CNT] = {
  {USART1, RCC_AHBPeriph_DMA1, DMA1_Channel5},
  {USART2, RCC_AHBPeriph_DMA1, DMA1_Channel6},
  {USART3, RCC_AHBPeriph_DMA1, DMA1_Channel3},
  {UART4,  RCC_AHBPeriph_DMA2, DMA2_Channel3},
  //{UART5,  -1, -1},  // UART5 don't support DMA
};

void Serial_DMA_Config(uint8_t port)
{
  const SERIAL_CFG * cfg = &Serial[port];

  RCC_AHBPeriphClockCmd(cfg->dma_rcc, ENABLE);  // DMA RCC EN

  cfg->dma_chanel->CCR &= ~(1<<0);              // DMA disable
  cfg->uart->CR3 |= 1<<6;                       // DMA enable receiver

  cfg->dma_chanel->CPAR = (uint32_t)(&cfg->uart->DR);
  cfg->dma_chanel->CMAR = (uint32_t)(dmaL1Data[port].cache);
  cfg->dma_chanel->CNDTR = dmaL1Data[port].cacheSize;
  cfg->dma_chanel->CCR = 0X00000000;
  cfg->dma_chanel->CCR |= 3<<12;  // Channel priority level
  cfg->dma_chanel->CCR |= 1<<7;   // Memory increment mode
  cfg->dma_chanel->CCR |= 1<<5;   // Circular mode enabled
  cfg->dma_chanel->CCR |= 1<<0;   // DMA EN
}

void Serial_ClearData(uint8_t port)
{
  dmaL1Data[port].rIndex = dmaL1Data[port].wIndex = dmaL1Data[port].cacheSize = 0;

  if (dmaL1Data[port].cache != NULL)
  {
    free(dmaL1Data[port].cache);
    dmaL1Data[port].cache = NULL;
  }

  infoHost.rx_ok[port] = false;
}

void Serial_Config(uint8_t port, uint16_t cacheSize, uint32_t baudrate)
{
  Serial_ClearData(port);

  dmaL1Data[port].cacheSize = cacheSize;
  dmaL1Data[port].cache = malloc(cacheSize);
  while (!dmaL1Data[port].cache);              // malloc failed

  UART_Config(port, baudrate, USART_IT_IDLE);  // IDLE interrupt
  Serial_DMA_Config(port);
}

void Serial_DeConfig(uint8_t port)
{
  Serial_ClearData(port);

  Serial[port].dma_chanel->CCR &= ~(1<<0);  // Disable DMA
  UART_DeConfig(port);
}

void USART_IRQHandler(uint8_t port)
{
  if ((Serial[port].uart->SR & (1<<4)) != 0)
  {
    Serial[port].uart->SR;
    Serial[port].uart->DR;

    dmaL1Data[port].wIndex = dmaL1Data[port].cacheSize - Serial[port].dma_chanel->CNDTR;
    uint16_t wIndex = (dmaL1Data[port].wIndex == 0) ? dmaL1Data[port].cacheSize : dmaL1Data[port].wIndex;
    if (dmaL1Data[port].cache[wIndex-1] == '\n')  // Receive completed
    {
      infoHost.rx_ok[port] = true;
    }
  }
}

void USART1_IRQHandler(void)
{
  USART_IRQHandler(_USART1);
}

void USART2_IRQHandler(void)
{
  USART_IRQHandler(_USART2);
}

void USART3_IRQHandler(void)
{
  USART_IRQHandler(_USART3);
}

void UART4_IRQHandler(void)
{
  USART_IRQHandler(_UART4);
}

void UART5_IRQHandler(void)
{
  USART_IRQHandler(_UART5);
}

void Serial_Puts(uint8_t port, char *s)
{
  while (*s)
  {
    while ((Serial[port].uart->SR & USART_FLAG_TC) == (uint16_t)RESET);
    Serial[port].uart->DR = ((u16)*s++ & (uint16_t)0x01FF);
  }
}

void Serial_Putchar(uint8_t port, char ch)
{
  while ((Serial[port].uart->SR & USART_FLAG_TC) == (uint16_t)RESET);
  Serial[port].uart->DR = (uint8_t) ch;
}
