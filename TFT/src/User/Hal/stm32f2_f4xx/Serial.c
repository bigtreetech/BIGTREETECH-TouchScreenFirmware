#include "Serial.h"
#include "includes.h"

// set this line to "true" to enable IDLE Line interrupt. It is no more needed, so always set this macro to "false"
#define IDLE_LINE_IT false

// uncomment this line to use TX DMA based serial writing. Otherwise only TX interrupt based serial writing will be used
#define TX_DMA_WRITE

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
SERIAL_CFG Serial[_UART_CNT] = {
// USART   DMA1 or DMA2         Channel RX Stream     TX Stream     TX Bytes
  {USART1, RCC_AHB1Periph_DMA2, 4,      DMA2_Stream2, DMA2_Stream7, 0},
  {USART2, RCC_AHB1Periph_DMA1, 4,      DMA1_Stream5, DMA1_Stream6, 0},
  {USART3, RCC_AHB1Periph_DMA1, 4,      DMA1_Stream1, DMA1_Stream3, 0},
  {UART4 , RCC_AHB1Periph_DMA1, 4,      DMA1_Stream2, DMA1_Stream4, 0},
  {UART5 , RCC_AHB1Periph_DMA1, 4,      DMA1_Stream0, DMA1_Stream7, 0},
  {USART6, RCC_AHB1Periph_DMA2, 5,      DMA2_Stream1, DMA2_Stream6, 0},
};

// clear all DMA RX and TX interrupt flags for a serial port
void Serial_DMAClearITflags(uint8_t port)
{
  // Channel to bits: Low  0: 0-5, 1: 6-11, 2: 16-21, 3: 22-27
  // Channel to bits: High 4: 0-5, 5: 6-11, 6: 16-21, 7: 22-27

  switch (port)  // DMA low/high interrupt Control Register (DMA_LIFCR/DMA_HIFCR)
  {
    case _USART1:
      DMA2->LIFCR = (0x3F << 16);   // DMA2_Stream2 low  bits:16-21 Channel 4
      DMA2->HIFCR = (0x3F << 22);   // DMA2_Stream7 high bits:22-27 Channel 4
      break;

    case _USART2:
      DMA1->HIFCR = (0x3F <<  6) |  // DMA1_Stream5 high bits: 6-11 Channel 4
                    (0x3F << 16);   // DMA1_Stream6 high bits:16-21 Channel 4
      break;

    case _USART3:
      DMA1->LIFCR = (0x3F <<  6) |  // DMA1_Stream1 low  bits: 6-11 Channel 4
                    (0x3F << 22);   // DMA1_Stream3 low  bits:22-27 Channel 4
      break;

    case _UART4:
      DMA1->LIFCR = (0x3F << 16);   // DMA1_Stream2 low  bits:16-21 Channel 4
      DMA1->HIFCR = (0x3F <<  0);   // DMA1_Stream4 high bits: 0- 5 Channel 4
      break;

    case _UART5:
      DMA1->LIFCR = (0x3F <<  0);   // DMA1_Stream0 low  bits: 0- 5 Channel 4
      DMA1->HIFCR = (0x3F << 22);   // DMA1_Stream7 high bits:22-27 Channel 4
      break;

    case _USART6:
      DMA2->LIFCR = (0x3F <<  6);   // DMA2_Stream1 low  bits: 6-11 Channel 5
      DMA2->HIFCR = (0x3F << 16);   // DMA2_Stream6 high bits:16-21 Channel 5
      break;
  }
}

// clear all DMA RX interrupt flags for a given serial port
void Serial_DMAClearITflagsRX(uint8_t port)
{
  // Channel to bits: Low  0: 0-5, 1: 6-11, 2: 16-21, 3: 22-27
  // Channel to bits: High 4: 0-5, 5: 6-11, 6: 16-21, 7: 22-27

  switch (port)  // DMA low/high interrupt Control Register (DMA_LIFCR/DMA_HIFCR)
  {
    case _USART1:
      DMA2->LIFCR = (0x3F << 16);  // DMA2_Stream2 low  bits:16-21 Channel 4
      break;

    case _USART2:
      DMA1->HIFCR = (0x3F << 6);   // DMA1_Stream5 high bits: 6-11 Channel 4
      break;

    case _USART3:
      DMA1->LIFCR = (0x3F << 6);   // DMA1_Stream1 low  bits: 6-11 Channel 4
      break;

    case _UART4:
      DMA1->LIFCR = (0x3F << 16);  // DMA1_Stream2 low  bits:16-21 Channel 4
      break;

    case _UART5:
      DMA1->LIFCR = (0x3F << 0);   // DMA1_Stream0 low  bits: 0-5 Channel 4
      break;

    case _USART6:
      DMA2->LIFCR = (0x3F << 6);   // DMA2_Stream1 low  bits: 6-11 Channel 5
      break;
  }
}

// clear all DMA TX interrupt flags for a given serial port
void Serial_DMAClearITflagsTX(uint8_t port)
{
  // Channel to bits: Low  0: 0-5, 1: 6-11, 2: 16-21, 3: 22-27
  // Channel to bits: High 4: 0-5, 5: 6-11, 6: 16-21, 7: 22-27

  switch (port)  // DMA low/high interrupt Control Register (DMA_LIFCR/DMA_HIFCR)
  {
    case _USART1:
      DMA2->HIFCR = (0x3F << 22);  // DMA2_Stream7 high bits:22-27 Channel 4
      break;

    case _USART2:
      DMA1->HIFCR = (0x3F << 16);  // DMA1_Stream6 high bits:16-21 Channel 4
      break;

    case _USART3:
      DMA1->LIFCR = (0x3F << 22);  // DMA1_Stream3 low  bits:22-27 Channel 4
      break;

    case _UART4:
      DMA1->HIFCR = (0x3F << 0);   // DMA1_Stream4 high bits: 0- 5 Channel 4
      break;

    case _UART5:
      DMA1->HIFCR = (0x3F << 22);  // DMA1_Stream7 high bits:22-27 Channel 4
      break;

    case _USART6:
      DMA2->HIFCR = (0x3F << 16);  // DMA2_Stream6 high bits:16-21 Channel 5
      break;
  }
}

void Serial_DMA_Config(uint8_t port)
{
  const SERIAL_CFG * cfg = &Serial[port];

  RCC_AHB1PeriphClockCmd(cfg->dma_rcc, ENABLE);           // DMA RCC enable

  cfg->dma_streamRX->CR &= ~(1<<0);                       // disable RX DMA
  cfg->dma_streamTX->CR &= ~(1<<0);                       // disable TX DMA

  Serial_DMAClearITflags(port);                           // clear DMA RX-TX interrupt flags

  cfg->uart->CR3 |= 3<<6;                                 // DMA enable transmitter (DMAT) and receiver (DMAR)

  cfg->dma_streamRX->PAR = (uint32_t)(&cfg->uart->DR);            // RX peripheral address (usart)
  cfg->dma_streamTX->PAR = (uint32_t)(&cfg->uart->DR);            // TX peripheral address (usart)

  cfg->dma_streamRX->M0AR = (uint32_t)(dmaL1DataRX[port].cache);  // RX destination data (sram)
  cfg->dma_streamRX->NDTR = dmaL1DataRX[port].cacheSize;          // RX buffer size

  cfg->dma_streamRX->CR = cfg->dma_channel << 25;         // RX channel selection, set to 0 all the other CR bits
  cfg->dma_streamTX->CR = cfg->dma_channel << 25;         // TX channel selection, set to 0 all the other CR bits

  if (port == SERIAL_PORT)
  { // primary serial port priority at highest level (TX higher than RX)
    cfg->dma_streamRX->CR |= 2<<16;                       // RX priority level: High
    cfg->dma_streamTX->CR |= 3<<16;                       // TX priority level: Very high
  }
  else
  { // secondary serial ports priority at medium level
    cfg->dma_streamRX->CR |= 0<<16;                       // RX priority level: Low
    cfg->dma_streamTX->CR |= 1<<16;                       // TX priority level: Medium
  }

//cfg->dma_streamRX->CR &= ~(3<<13);                      // RX memory data size: 8 bit
//cfg->dma_streamTX->CR &= ~(3<<13);                      // TX memory data size: 8 bit

//cfg->dma_streamRX->CR &= ~(3<<11);                      // RX peripheral data size: 8 bit
//cfg->dma_streamTX->CR &= ~(3<<11);                      // TX peripheral data size: 8 bit

  cfg->dma_streamRX->CR |= 1<<10;                         // RX memory increment mode
  cfg->dma_streamTX->CR |= 1<<10;                         // TX memory increment mode

//cfg->dma_streamRX->CR &= ~(1<<9);                       // peripheral no increment mode
//cfg->dma_streamTX->CR &= ~(1<<9);                       // peripheral no increment mode

  cfg->dma_streamRX->CR |= 1<<8;                          // RX circular mode enabled
//cfg->dma_streamTX->CR &= ~(1<<8);                       // TX circular mode disabled

//cfg->dma_streamRX->CR &= ~(1<<6);                       // RX data transfer direction: Peripheral-to-memory
  cfg->dma_streamTX->CR |= 1<<6;                          // TX data transfer direction: Memory-to-Peripheral

//cfg->dma_streamTX->CR |= 1<<4;                          // enable TX DMA Transfer Complete interrupt, done later when needed

  cfg->dma_streamRX->CR |= 1<<0;                          // re-enable RX DMA
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

  Serial[port].txBytes = 0;
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

  Serial[port].dma_streamRX->CR &= ~(1<<0);  // disable RX DMA
  Serial[port].dma_streamTX->CR &= ~(1<<0);  // disable TX DMA

  Serial_DMAClearITflags(port);
  UART_DeConfig(port);
}

#ifdef DEFAULT_WRITE  // unbuffered TX serial writing

void Serial_PutChar(uint8_t port, const char ch)
{
  while ((Serial[port].uart->SR & USART_SR_TC) == (uint16_t)RESET);
  Serial[port].uart->DR = (uint8_t) ch;
}

void Serial_Put(uint8_t port, const char * msg)
{
  while (*msg)
  {
    while ((Serial[port].uart->SR & USART_SR_TC) == (uint16_t)RESET);
    Serial[port].uart->DR = ((uint16_t)*msg++ & (uint16_t)0x01FF);
  }
}

#elif !defined(TX_DMA_WRITE)  // TX interrupt based serial writing

void Serial_PutChar(uint8_t port, const char ch)
{
  // wait for enough free buffer in TX cache to store all data
  while (((dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize) == dmaL1DataTX[port].rIndex)
  {
  }

  dmaL1DataTX[port].cache[dmaL1DataTX[port].wIndex] = ch;
  dmaL1DataTX[port].wIndex = (dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize;

  Serial[port].uart->CR1 |= USART_CR1_TXEIE;  // set TXE interrupt bit to start the serial transfer
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

    Serial[port].uart->CR1 |= USART_CR1_TXEIE;  // set TXE interrupt bit to start the serial transfer
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

  Serial[port].uart->CR1 |= USART_CR1_TXEIE;  // set TXE interrupt bit to start the serial transfer
#endif  // USE_COMPACT_CODE
}

#else  // TX DMA based serial writing

// DMA Serial Write support function
void Serial_Send_TX(uint8_t port)
{
  // setup DMA transfer, and wait for serial Transfer Complete (TX) interrupt in ISR
  if (dmaL1DataTX[port].wIndex >= dmaL1DataTX[port].rIndex)
    Serial[port].txBytes = dmaL1DataTX[port].wIndex - dmaL1DataTX[port].rIndex;
  else
    Serial[port].txBytes = dmaL1DataTX[port].cacheSize - dmaL1DataTX[port].rIndex;                   // split transfer into 2 parts

  Serial[port].dma_streamTX->M0AR = (uint32_t)(&dmaL1DataTX[port].cache[dmaL1DataTX[port].rIndex]);  // start address TX data
  Serial_DMAClearITflagsTX(port);                          // clear DMA TX interrupt flags
  Serial[port].uart->CR1 |= USART_CR1_TCIE;                // enable Transfer Complete (TX) serial Interrupt
  Serial[port].dma_streamTX->NDTR = Serial[port].txBytes;  // no. bytes to transfer
  Serial[port].dma_streamTX->CR |= 1<<0;                   // enable TX DMA
}

void Serial_PutChar(uint8_t port, const char ch)
{
  // setup TX DMA, if no '\n' yet, but buffer is full AND DMA is not in progress already (waiting for Transfer Complete interrupt)
  if ((((dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize) == dmaL1DataTX[port].rIndex) &&
      ((Serial[port].uart->CR1 & USART_CR1_TCIE) == 0))
    Serial_Send_TX(port);

  // wait until space becomes available, blocking!
  while (((dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize) == dmaL1DataTX[port].rIndex) { }

  dmaL1DataTX[port].cache[dmaL1DataTX[port].wIndex] = ch;                                   // copy character to cache
  dmaL1DataTX[port].wIndex = (dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize;  // update wIndex

  if ((ch == '\n') && ((Serial[port].uart->CR1 & USART_CR1_TCIE) == 0))
    Serial_Send_TX(port);  // start DMA process if command is complete and DMA is not in progress already
}

void Serial_Put(uint8_t port, const char * msg)
{
  while (*msg)
  {
    // setup TX DMA, if no '\n' yet, but buffer is full AND DMA is not in progress already (waiting for Transfer Complete interrupt)
    if ((((dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize) == dmaL1DataTX[port].rIndex) &&
        ((Serial[port].uart->CR1 & USART_CR1_TCIE) == 0))
      Serial_Send_TX(port);

    // wait until space becomes available, blocking!
    while (((dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize) == dmaL1DataTX[port].rIndex) { }

    dmaL1DataTX[port].cache[dmaL1DataTX[port].wIndex] = *msg;                                 // copy character to cache
    dmaL1DataTX[port].wIndex = (dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize;  // update wIndex

    if ((*msg == '\n') && ((Serial[port].uart->CR1 & USART_CR1_TCIE) == 0))
      Serial_Send_TX(port);  // start DMA process if command is complete and DMA is not in progress already

    msg++;  // let the compiler optimize this, no need to do it manually!
  }
}

#endif

// ISR, serial interrupt handler
void USART_IRQHandler(uint8_t port)
{
#if IDLE_LINE_IT == true  // IDLE Line interrupt
  if ((Serial[port].uart->SR & USART_SR_IDLE) != RESET)  // check for IDLE Line interrupt
  {
    Serial[port].uart->SR;                               // clear IDLE Line bit
    Serial[port].uart->DR;

    dmaL1DataRX[port].wIndex = dmaL1DataRX[port].cacheSize - Serial[port].dma_streamRX->NDTR;
  }
#endif

#ifndef DEFAULT_WRITE

#ifndef TX_DMA_WRITE  // TX interrupt based serial writing

  if ((Serial[port].uart->SR & USART_SR_TXE) != 0)             // check for TXE interrupt
  {
    if (dmaL1DataTX[port].rIndex == dmaL1DataTX[port].wIndex)  // no more data?
    {
      Serial[port].uart->CR1 &= ~USART_CR1_TXEIE;              // disable TXE interrupt
    }
    else
    {
      Serial[port].uart->DR = (uint8_t)dmaL1DataTX[port].cache[dmaL1DataTX[port].rIndex];       // write next available character

      dmaL1DataTX[port].rIndex = (dmaL1DataTX[port].rIndex + 1) % dmaL1DataTX[port].cacheSize;  // increase reading index
    }
  }

#else  // TX DMA based serial writing

  if ((Serial[port].uart->SR & USART_SR_TC) != 0)  // check for Transfer Complete (TC) interrupt
  {
    Serial[port].uart->SR &= ~USART_SR_TC;         // clear Transfer Complete (TC) bit

    // NOTE: the ISR is sometimes called while DMA is still active, so check NDTR status!
    //
    if (Serial[port].dma_streamTX->NDTR == 0)      // sending is complete
    {
      dmaL1DataTX[port].rIndex = (dmaL1DataTX[port].rIndex + Serial[port].txBytes) % dmaL1DataTX[port].cacheSize;
      Serial[port].txBytes = 0;

      if (dmaL1DataTX[port].wIndex != dmaL1DataTX[port].rIndex)  // is more data available?
        Serial_Send_TX(port);                                    // continue sending data
      else                                                       // removed causes double line transfers
        Serial[port].uart->CR1 &= ~USART_CR1_TCIE;               // disable Transfer Complete (TC) interrupt, nothing more to do
    }
    // else: more data is coming, wait for next Transfer Complete (TC) interrupt
  }

#endif  // TX_DMA_WRITE

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

void USART6_IRQHandler(void)
{
  USART_IRQHandler(_USART6);
}
