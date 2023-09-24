#include "Serial.h"
#include "includes.h"  // for infoHost

// set this line to "true" to enable serial IDLE interrupt. IDLE interrupt is no more needed, so always set this macro to "false"
#define RX_IDLE_INTERRUPT false

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

// Config for USART Channel
const SERIAL_CFG Serial[_UART_CNT] = {
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
  cfg->dma_chanel->CMAR = (uint32_t)(dmaL1DataRX[port].cache);
  cfg->dma_chanel->CNDTR = dmaL1DataRX[port].cacheSize;
  cfg->dma_chanel->CCR = 0X00000000;
  cfg->dma_chanel->CCR |= 3<<12;  // Channel priority level
  cfg->dma_chanel->CCR |= 1<<7;   // Memory increment mode
  cfg->dma_chanel->CCR |= 1<<5;   // Circular mode enabled
  cfg->dma_chanel->CCR |= 1<<0;   // DMA EN
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
  while (!dmaL1DataRX[port].cache);               // malloc failed, blocking!

  dmaL1DataTX[port].cacheSize = cacheSizeTX;
  dmaL1DataTX[port].cache = malloc(cacheSizeTX);
  while (!dmaL1DataTX[port].cache);               // malloc failed, blocking!

  UART_Config(port, baudrate, USART_IT_IDLE, RX_IDLE_INTERRUPT);  // configure serial line with or without IDLE interrupt
  Serial_DMA_Config(port);
}

void Serial_DeConfig(uint8_t port)
{
  Serial_ClearData(port);

  Serial[port].dma_chanel->CCR &= ~(1<<0);  // Disable DMA
  UART_DeConfig(port);
}

#ifdef DEFAULT_WRITE  // unbuffered TX serial writing

void Serial_PutChar(uint8_t port, const char ch)
{
  while ((Serial[port].uart->SR & USART_FLAG_TC) == (uint16_t)RESET);
  Serial[port].uart->DR = (uint8_t) ch;
}

void Serial_Put(uint8_t port, const char * msg)
{
  while (*msg)
  {
    while ((Serial[port].uart->SR & USART_FLAG_TC) == (uint16_t)RESET);
    Serial[port].uart->DR = ((uint16_t)*msg++ & (uint16_t)0x01FF);
  }
}

#else  // use TX interrupt based serial writing

void Serial_PutChar(uint8_t port, const char ch)
{
  // wait for enough free buffer in TX cache to store all data
  while (((dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize) == dmaL1DataTX[port].rIndex)
  {
  }

  dmaL1DataTX[port].cache[dmaL1DataTX[port].wIndex] = ch;
  dmaL1DataTX[port].wIndex = (dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize;

  Serial[port].uart->CR1 |= USART_FLAG_TXE;  // set TXE interrupt bit to start the serial transfer
}

void Serial_Put(uint8_t port, const char * msg)
{
#ifdef USE_COMPACT_CODE  // less code
  while (*msg)
  {
    // blocking! wait for buffer to become available
    while (((dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize) == dmaL1DataTX[port].rIndex) { };

    dmaL1DataTX[port].cache[dmaL1DataTX[port].wIndex] = *msg++;
    dmaL1DataTX[port].wIndex = (dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize;

    Serial[port].uart->CR1 |= USART_FLAG_TXE;  // set TXE interrupt bit to start the serial transfer
  }
#else  // fast code
  // NOTE: used 32 bit variables for performance reasons (in particular for data copy)

  DMA_CIRCULAR_BUFFER * dmaL1Data_ptr = &dmaL1DataTX[port];
  char * cache = dmaL1Data_ptr->cache;
  uint32_t cacheSize = dmaL1Data_ptr->cacheSize;
  uint32_t wIndex = dmaL1Data_ptr->wIndex;
  uint32_t msgSize = strlen(msg);

  // if cache size is not enough to store the data, skip the data copy
  //
  // NOTE: the following check should never be matched if cache has a proper size.
  //       If so, the check could be commented out just to improve performance. Just keep it to make the code more robust
  //
  if ((cacheSize - 1) < msgSize)
    return;

  // wait for enough free buffer in TX cache to store all data.
  // Used dmaL1Data_ptr->rIndex dynamically changed by TX cache's interrupt handler
  //
  // NOTE: -1 is needed because full cache usage will introduce a conflict on rIndex and wIndex
  //       (wIndex == rIndex will indicate an empty cache or a full cache)
  //
  while ((((cacheSize - 1) - wIndex + dmaL1Data_ptr->rIndex) % cacheSize) < msgSize)
  {
  }

  uint32_t maxIndex;

  // if data is one chunk only, store data on upper part of circular cache
  if ((cacheSize - wIndex) >= msgSize)
  {
  #ifdef USE_INLINE_COPY
    maxIndex = wIndex + msgSize;

    while (wIndex < maxIndex)
    {
      cache[wIndex++] = *(msg++);
    }
  #else
    memcpy(&cache[wIndex], msg, msgSize);
  #endif
  }
  else  // data at end and beginning of cache
  {
  #ifdef USE_INLINE_COPY
    while (wIndex < cacheSize)
    {
      cache[wIndex++] = *(msg++);
    }

    wIndex = 0;
    maxIndex = msgSize - (cacheSize - dmaL1Data_ptr->wIndex);  // used dmaL1Data_ptr->wIndex and not wIndex

    while (wIndex < maxIndex)
    {
      cache[wIndex++] = *(msg++);
    }
  #else
    memcpy(&cache[wIndex], msg, cacheSize - wIndex);
    memcpy(cache, &msg[cacheSize - wIndex], msgSize - (cacheSize - wIndex));
  #endif
  }

  // update queue's writing index with next index
  dmaL1Data_ptr->wIndex = (dmaL1Data_ptr->wIndex + msgSize) % cacheSize;

  Serial[port].uart->CR1 |= USART_FLAG_TXE;  // set TXE interrupt bit to start the serial transfer
#endif  // USE_COMPACT_CODE
}

#endif

// ISR, serial interrupt handler
void USART_IRQHandler(uint8_t port)
{
#if RX_IDLE_INTERRUPT == true  // RX serial IDLE interrupt
  if ((Serial[port].uart->SR & USART_FLAG_IDLE) != 0)  // RX: check for serial IDLE interrupt
  {
    //Serial[port].uart->SR;  // already done in the guard above
    Serial[port].uart->DR;

    dmaL1DataRX[port].wIndex = dmaL1DataRX[port].cacheSize - Serial[port].dma_chanel->CNDTR;
  }
#endif

#ifndef DEFAULT_WRITE  // TX interrupt based serial writing

  if ((Serial[port].uart->SR & USART_FLAG_TXE) != 0)  // TX: check for TXE interrupt
  {
    if (dmaL1DataTX[port].rIndex == dmaL1DataTX[port].wIndex)  // no more data?
    {
      Serial[port].uart->CR1 &= ~USART_FLAG_TXE;               // disable TXE interrupt
    }
    else
    {
      Serial[port].uart->DR = (uint8_t)dmaL1DataTX[port].cache[dmaL1DataTX[port].rIndex];       // write next available character

      dmaL1DataTX[port].rIndex = (dmaL1DataTX[port].rIndex + 1) % dmaL1DataTX[port].cacheSize;  // increase reading index
    }
  }

#endif  // DEFAULT_WRITE
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
