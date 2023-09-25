#include "Serial.h"
#include "includes.h"

// set this line to "true" to enable IDLE Line interrupt. It is no more needed, so always set this macro to "false"
#define IDLE_LINE_IT false

// uncomment this line to use TX DMA based serial writing. Otherwise only TX interrupt based serial writing will be used
//#define TX_DMA_WRITE

// uncomment this line to use compact code (less code) instead of fast code
#define USE_COMPACT_CODE

#ifndef USE_COMPACT_CODE
  // uncomment this line to use inline copy (fast code) instead of memcpy() (less code)
  #define USE_INLINE_COPY
#endif

//#define DEFAULT_WRITE  // old unbuffered TX serial writing (just for comparison with new TX solutions)

DMA_CIRCULAR_BUFFER dmaL1DataRX[_UART_CNT] = {0};  // DMA RX buffer
DMA_CIRCULAR_BUFFER dmaL1DataTX[_UART_CNT] = {0};  // DMA TX buffer

// config for USART channel
const SERIAL_CFG Serial[_UART_CNT] = {
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
    case _USART2: DMA_INTC(DMA0) = (0x0F << 20); break;
    case _USART3: DMA_INTC(DMA0) = (0x0F << 8);  break;
    case _UART4:  DMA_INTC(DMA1) = (0x0F << 8);  break;
    case _UART5:  DMA_INTC(DMA1) = (0x0F << 4);  break;
    case _USART6: DMA_INTC(DMA1) = (0x0F << 20); break;
  }
}

void Serial_DMA_Config(uint8_t port)
{
  const SERIAL_CFG * cfg = &Serial[port];

  rcu_periph_clock_enable(cfg->dma_rcc);

  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) &= ~(1<<0);  // Disable DMA
  Serial_DMAClearFlag(port);
  USART_CTL2(cfg->uart) |= 1<<6;                            // DMA enable receiver

  DMA_CHPADDR(cfg->dma_stream, cfg->dma_channel) = (uint32_t)(&USART_DATA(cfg->uart));
  DMA_CHMADDR(cfg->dma_stream, cfg->dma_channel) = (uint32_t)(dmaL1DataRX[port].cache);
  DMA_CHCNT(cfg->dma_stream, cfg->dma_channel) = dmaL1DataRX[port].cacheSize;

  // DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) = cfg->dma_channel << 25;
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 3<<12;  // Priority level: Very high
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 0<<10;  // Memory data size: 8
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 0<<8;   // Peripheral data size: 8
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 1<<7;   // Memory increment mode
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 0<<6;   // Peripheral not increment mode
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 1<<5;   // Circular mode enabled
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 0<<4;   // Data transfer direction: Peripheral-to-memory
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channel) |= 1<<0;   // Enable DMA
}

void Serial_ClearData(uint8_t port)
{
  dmaL1DataRX[port].wIndex = dmaL1DataRX[port].rIndex = dmaL1DataRX[port].flag = dmaL1DataRX[port].cacheSize = 0;

  if (dmaL1DataRX[port].cache != NULL)
  {
    free(dmaL1DataRX[port].cache);
    dmaL1DataRX[port].cache = NULL;
  }

  dmaL1DataTX[port].wIndex = dmaL1DataTX[port].rIndex = dmaL1DataTX[port].flag = dmaL1DataTX[port].cacheSize = 0;

  if (dmaL1DataTX[port].cache != NULL)
  {
    free(dmaL1DataTX[port].cache);
    dmaL1DataTX[port].cache = NULL;
  }
}

void Serial_Config(uint8_t port, uint16_t cacheSizeRX, uint16_t cacheSizeTX, uint32_t baudrate)
{
  Serial_ClearData(port);

  dmaL1DataRX[port].cacheSize = cacheSizeRX;
  dmaL1DataRX[port].cache = malloc(cacheSizeRX);
  while (!dmaL1DataRX[port].cache);                          // malloc failed, blocking!

  dmaL1DataTX[port].cacheSize = cacheSizeTX;
  dmaL1DataTX[port].cache = malloc(cacheSizeTX);
  while (!dmaL1DataTX[port].cache);                          // malloc failed, blocking!

  UART_Config(port, baudrate, USART_IT_IDLE, IDLE_LINE_IT);  // configure serial line with or without IDLE Line interrupt
  Serial_DMA_Config(port);
}

void Serial_DeConfig(uint8_t port)
{
  Serial_ClearData(port);

  DMA_CHCTL(Serial[port].dma_stream, Serial[port].dma_channel) &= ~(1<<0);  // Disable DMA
  Serial_DMAClearFlag(port);
  UART_DeConfig(port);
}

void Serial_PutChar(uint8_t port, const char ch)
{
  while ((USART_STAT0(Serial[port].uart) & (1 << USART_FLAG_TC)) == (uint16_t)RESET);
  USART_DATA(Serial[port].uart) = (uint8_t) ch;
}

void Serial_Put(uint8_t port, const char * msg)
{
  while (*msg)
  {
    while ((USART_STAT0(Serial[port].uart) & (1 << USART_FLAG_TC)) == (uint16_t)RESET);
    USART_DATA(Serial[port].uart) = ((uint16_t)*msg++ & (uint16_t)0x01FF);
  }
}

// ISR, serial interrupt handler
void USART_IRQHandler(uint8_t port)
{
#if IDLE_LINE_IT == true  // IDLE Line interrupt
  if ((USART_STAT0(Serial[port].uart) & (1<<4)) != 0)  // check for IDLE Line interrupt
  {
    USART_STAT0(Serial[port].uart);                    // clear IDLE Line bit
    USART_DATA(Serial[port].uart);

    dmaL1DataRX[port].wIndex = dmaL1DataRX[port].cacheSize - DMA_CHCNT(Serial[port].dma_stream, Serial[port].dma_channel);
  }
#endif
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
