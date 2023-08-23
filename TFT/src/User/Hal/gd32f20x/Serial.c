#include "Serial.h"
#include "includes.h"  // for infoHost

// dma rx buffer
DMA_CIRCULAR_BUFFER dmaL1Data[_UART_CNT] = {0};

// Config for USART Channel
//USART1 RX DMA2 Channel4 Steam2/5
//USART2 RX DMA1 Channel4 Steam5
//USART3 RX DMA1 Channel4 Steam1
//UART4  RX DMA1 Channel4 Steam2
//UART5  RX DMA1 Channel4 Steam0
//USART6 RX DMA2 Channel5 Steam1/2

// Config for USART Channel
typedef struct
{
  uint32_t uart;
  rcu_periph_enum dma_rcc;
  uint8_t dma_channel;
  uint32_t dma_stream;
} SERIAL_CFG;

static const SERIAL_CFG Serial[_UART_CNT] = {
  {USART0, RCU_DMA0, 4, DMA0},
  {USART1, RCU_DMA0, 5, DMA0},
  {USART2, RCU_DMA0, 2, DMA0},
  {UART3,  RCU_DMA1, 2, DMA1},
  {UART4,  RCU_DMA1, 1, DMA1},
  {USART5, RCU_DMA1, 5, DMA1},
};

void Serial_DMAClearFlag(uint8_t port)
{
  switch(port)
  {
    case _USART1: DMA_INTC(DMA0) = (0x0F << 12); break;
    case _USART2: DMA_INTC(DMA0) = (0x0F << 20);  break;
    case _USART3: DMA_INTC(DMA0) = (0x0F << 8);  break;
    case _UART4:  DMA_INTC(DMA1) = (0x0F << 8); break;
    case _UART5:  DMA_INTC(DMA1) = (0x0F << 4);  break;
    case _USART6: DMA_INTC(DMA1) = (0x0F << 20);  break;
  }
}

void Serial_DMA_Config(uint8_t port)
{
  const SERIAL_CFG * cfg = &Serial[port];
  rcu_periph_clock_enable(cfg->dma_rcc);

  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) &= ~(1<<0);   // Disable DMA
  Serial_DMAClearFlag(port);
  USART_CTL2(cfg->uart) |= 1<<6;                        // DMA enable receiver

  DMA_CHPADDR(cfg->dma_stream, cfg->dma_channel) = (uint32_t)(&USART_DATA(cfg->uart));
  DMA_CHMADDR(cfg->dma_stream, cfg->dma_channel) = (uint32_t)(dmaL1Data[port].cache);
  DMA_CHCNT(cfg->dma_stream, cfg->dma_channel) = dmaL1Data[port].cacheSize;

  // DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) = cfg->dma_channel << 25;
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 3<<12;  // Priority level: Very high
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 0<<10;  // Memory data size: 8
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 0<<8;  // Peripheral data size: 8
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 1<<7;  // Memory increment mode
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 0<<6;   // Peripheral not increment mode
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 1<<5;   // Circular mode enabled
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 0<<4;   // Data transfer direction: Peripheral-to-memory
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 1<<0;   // Enable DMA
}

void Serial_ClearData(uint8_t port)
{
  dmaL1Data[port].wIndex = dmaL1Data[port].rIndex = dmaL1Data[port].flag = dmaL1Data[port].cacheSize = 0;

  if (dmaL1Data[port].cache != NULL)
  {
    free(dmaL1Data[port].cache);
    dmaL1Data[port].cache = NULL;
  }
}

void Serial_Config(uint8_t port, uint16_t cacheSize, uint32_t baudrate)
{
  Serial_ClearData(port);

  dmaL1Data[port].cacheSize = cacheSize;
  dmaL1Data[port].cache = malloc(cacheSize);
  while (!dmaL1Data[port].cache);              // malloc failed

  UART_Config(port, baudrate, USART_INT_IDLE);  // IDLE interrupt
  Serial_DMA_Config(port);
}

void Serial_DeConfig(uint8_t port)
{
  Serial_ClearData(port);

  DMA_CHCTL(Serial[port].dma_stream, Serial[port].dma_channel) &= ~(1<<0);  // Disable DMA
  Serial_DMAClearFlag(port);
  UART_DeConfig(port);
}

void USART_IRQHandler(uint8_t port)
{
  if ((USART_STAT0(Serial[port].uart) & (1<<4)) != 0)
  {
    USART_STAT0(Serial[port].uart);  // Clear interrupt flag
    USART_DATA(Serial[port].uart);

    dmaL1Data[port].wIndex = dmaL1Data[port].cacheSize - DMA_CHCNT(Serial[port].dma_stream, Serial[port].dma_channel);
  }
}

void USART0_IRQHandler(void)
{
  USART_IRQHandler(_USART1);
}

void USART1_IRQHandler(void)
{
  USART_IRQHandler(_USART2);
}

void USART2_IRQHandler(void)
{
  USART_IRQHandler(_USART3);
}

void UART3_IRQHandler(void)
{
  USART_IRQHandler(_UART4);
}

void UART4_IRQHandler(void)
{
  USART_IRQHandler(_UART5);
}

void USART5_IRQHandler(void)
{
  USART_IRQHandler(_USART6);
}

void Serial_Put(uint8_t port, const char *s)
{
  while (*s)
  {
    while ((USART_STAT0(Serial[port].uart) & (1 << USART_FLAG_TC)) == (uint16_t)RESET);
    USART_DATA(Serial[port].uart) = ((uint16_t)*s++ & (uint16_t)0x01FF);
  }
}

void Serial_PutChar(uint8_t port, const char ch)
{
  while ((USART_STAT0(Serial[port].uart) & (1 << USART_FLAG_TC)) == (uint16_t)RESET);
  USART_DATA(Serial[port].uart) = (uint8_t) ch;
}
