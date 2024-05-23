#include "Serial.h"
#include "includes.h"

// set this line to "true" to enable IDLE Line interrupt. It is no more needed, so always set this macro to "false"
#define IDLE_LINE_IT false

// DMA registers:
//
// CR     DMA stream x configuration register
// NDTR   DMA stream x number of data register
// PAR    DMA stream x peripheral address register
// M0AR   DMA stream x memory 0 address register
//
// LISR   DMA low interrupt status register
// HISR   DMA high interrupt status register
// LIFCR  DMA low interrupt flag clear register
// HIFCR  DMA high interrupt flag clear register

DMA_CIRCULAR_BUFFER dmaL1DataRX[_UART_CNT] = {0};  // DMA / interrupt RX buffer
DMA_CIRCULAR_BUFFER dmaL1DataTX[_UART_CNT] = {0};  // DMA / interrupt TX buffer

// config for USART DMA channels
const SERIAL_CFG Serial[_UART_CNT] = {
  #ifdef TX_DMA_WRITE
  // USART   TCC            DMAx  Channel RX Stream     TX Stream
    {USART1, RCC_AHB1Periph_DMA2, 4,      DMA2_Stream2, DMA2_Stream7},
    {USART2, RCC_AHB1Periph_DMA1, 4,      DMA1_Stream5, DMA1_Stream6},
    {USART3, RCC_AHB1Periph_DMA1, 4,      DMA1_Stream1, DMA1_Stream3},
    {UART4,  RCC_AHB1Periph_DMA1, 4,      DMA1_Stream2, DMA1_Stream4},
    {UART5,  RCC_AHB1Periph_DMA1, 4,      DMA1_Stream0, DMA1_Stream7},
    {USART6, RCC_AHB1Periph_DMA2, 5,      DMA2_Stream1, DMA2_Stream6},
  #else
  // USART   TCC            DMAx  Channel RX Stream
    {USART1, RCC_AHB1Periph_DMA2, 4,      DMA2_Stream2},
    {USART2, RCC_AHB1Periph_DMA1, 4,      DMA1_Stream5},
    {USART3, RCC_AHB1Periph_DMA1, 4,      DMA1_Stream1},
    {UART4,  RCC_AHB1Periph_DMA1, 4,      DMA1_Stream2},
    {UART5,  RCC_AHB1Periph_DMA1, 4,      DMA1_Stream0},
    {USART6, RCC_AHB1Periph_DMA2, 5,      DMA2_Stream1},
  #endif
};

// disable RX DMA and clear all interrupt flags for a serial port
static void Serial_DMA_DisableAndClearFlagsRX(uint8_t port)
{
  Serial[port].dma_streamRX->CR &= ~(1<<0);  // disable RX DMA

  // Stream # to bits: Low  0: 0-5, 1: 6-11, 2: 16-21, 3: 22-27
  // Stream # to bits: High 4: 0-5, 5: 6-11, 6: 16-21, 7: 22-27
  //
  // DMA low/high interrupt Control Register (DMA_LIFCR/DMA_HIFCR)

  switch (port)
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

#ifdef TX_DMA_WRITE

// disable TX DMA and clear all interrupt flags for a serial port
static void Serial_DMA_DisableAndClearFlagsTX(uint8_t port)
{
  Serial[port].dma_streamTX->CR &= ~(1<<0);  // disable TX DMA

  // Stream # to bits: Low  0: 0-5, 1: 6-11, 2: 16-21, 3: 22-27
  // Stream # to bits: High 4: 0-5, 5: 6-11, 6: 16-21, 7: 22-27
  //
  // DMA low/high interrupt Control Register (DMA_LIFCR/DMA_HIFCR)

  switch (port)
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

#endif

static inline void Serial_DMA_Config(uint8_t port)
{
  const SERIAL_CFG * cfg = &Serial[port];

  RCC_AHB1PeriphClockCmd(cfg->dma_rcc, ENABLE);                   // enable DMA clock

  Serial_DMA_DisableAndClearFlagsRX(port);                        // RX disable DMA and clear all interrupt flags

  cfg->dma_streamRX->PAR = (uint32_t)(&cfg->uart->DR);            // RX peripheral address (usart)
  cfg->dma_streamRX->M0AR = (uint32_t)(dmaL1DataRX[port].cache);  // RX destination address (memory)
  cfg->dma_streamRX->NDTR = dmaL1DataRX[port].cacheSize;          // RX buffer size

  cfg->dma_streamRX->CR = (cfg->dma_channel << 25);               // RX channel selection, set to 0 all the other CR bits

  // primary serial port priority at highest level (TX higher than RX)
  if (port == SERIAL_PORT)
    cfg->dma_streamRX->CR |= (2<<16);                             // RX priority level: High
  else
    cfg->dma_streamRX->CR |= (0<<16);                             // RX priority level: Low

  //cfg->dma_streamRX->CR &= ~(3<<13);                            // RX memory data size: 8 bit
  //cfg->dma_streamRX->CR &= ~(3<<11);                            // RX peripheral data size: 8 bit
  cfg->dma_streamRX->CR |= (1<<10);                               // RX memory increment mode
  //cfg->dma_streamRX->CR &= ~(1<<9);                             // RX peripheral no increment mode
  cfg->dma_streamRX->CR |= (1<<8);                                // RX circular mode enabled
  //cfg->dma_streamRX->CR &= ~(1<<6);                             // RX data transfer direction: Peripheral-to-memory

  #ifdef TX_DMA_WRITE  // TX DMA based serial writing
    Serial_DMA_DisableAndClearFlagsTX(port);                      // TX disable DMA and clear all interrupt flags

    cfg->dma_streamTX->PAR = (uint32_t)(&cfg->uart->DR);          // TX peripheral address (usart)

    cfg->dma_streamTX->CR = (cfg->dma_channel << 25);             // TX channel selection, set to 0 all the other CR bits

    // primary serial port priority at highest level (TX higher than RX)
    if (port == SERIAL_PORT)
      cfg->dma_streamTX->CR |= (3<<16);                           // TX priority level: Very high
    else
      cfg->dma_streamTX->CR |= (1<<16);                           // TX priority level: Medium

    //cfg->dma_streamTX->CR &= ~(3<<13);                          // TX memory data size: 8 bit
    //cfg->dma_streamTX->CR &= ~(3<<11);                          // TX peripheral data size: 8 bit
    cfg->dma_streamTX->CR |= (1<<10);                             // TX memory increment mode
    //cfg->dma_streamTX->CR &= ~(1<<9);                           // TX peripheral no increment mode
    //cfg->dma_streamTX->CR &= ~(1<<8);                           // TX circular mode disabled
    cfg->dma_streamTX->CR |= (1<<6);                              // TX data transfer direction: Memory-to-Peripheral

    cfg->uart->CR3 |= (1<<7);                                     // enable DMA transmitter (DMAT)
    //cfg->dma_streamTX->CR |= (1<<0);                            // TX enable DMA, done later when needed
  #endif

  cfg->uart->CR3 |= (1<<6);                                       // enable DMA receiver (DMAR)
  cfg->dma_streamRX->CR |= (1<<0);                                // RX enable DMA
}

static void Serial_ClearData(uint8_t port)
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

void Serial_Config(uint8_t port, uint32_t cacheSizeRX, uint32_t cacheSizeTX, uint32_t baudrate)
{
  Serial_ClearData(port);

  dmaL1DataRX[port].cacheSize = cacheSizeRX;
  dmaL1DataRX[port].cache = malloc(cacheSizeRX);
  while (!dmaL1DataRX[port].cache);                          // RX malloc failed, blocking!

  dmaL1DataTX[port].cacheSize = cacheSizeTX;
  dmaL1DataTX[port].cache = malloc(cacheSizeTX);
  while (!dmaL1DataTX[port].cache);                          // TX malloc failed, blocking!

  UART_Config(port, baudrate, USART_IT_IDLE, IDLE_LINE_IT);  // configure serial line with or without IDLE Line interrupt

  Serial_DMA_Config(port);
}

void Serial_DeConfig(uint8_t port)
{
  Serial_ClearData(port);

  Serial_DMA_DisableAndClearFlagsRX(port);    // disable RX DMA and clear all interrupt flags

  #ifdef TX_DMA_WRITE
    Serial_DMA_DisableAndClearFlagsTX(port);  // disable TX DMA and clear all interrupt flags
  #endif

  UART_DeConfig(port);
}

#ifdef TX_DMA_WRITE  // TX DMA based serial writing

// DMA serial write support function
static void Serial_Send_TX(uint8_t port)
{
  // setup DMA transfer, and wait for serial Transfer Complete (TX) interrupt in ISR
  if (dmaL1DataTX[port].wIndex >= dmaL1DataTX[port].rIndex)
    dmaL1DataTX[port].flag = dmaL1DataTX[port].wIndex - dmaL1DataTX[port].rIndex;
  else  // split transfer into 2 parts
    dmaL1DataTX[port].flag = dmaL1DataTX[port].cacheSize - dmaL1DataTX[port].rIndex;

  Serial_DMA_DisableAndClearFlagsTX(port);   // disable TX DMA and clear all interrupt flags

  Serial[port].dma_streamTX->M0AR = (uint32_t)(&dmaL1DataTX[port].cache[dmaL1DataTX[port].rIndex]);  // TX data start address
  Serial[port].dma_streamTX->NDTR = dmaL1DataTX[port].flag;                                          // number of bytes to transfer

  Serial[port].uart->CR1 |= USART_CR1_TCIE;  // enable Transfer Complete (TC) serial interrupt
  Serial[port].dma_streamTX->CR |= (1<<0);   // enable TX DMA
}

void Serial_Put(uint8_t port, const char * msg)
{
  while (*msg)
  {
    // setup TX DMA, if no '\n' yet, but buffer is full AND DMA is not in progress already (waiting for Transfer Complete interrupt)
    if ((((dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize) == dmaL1DataTX[port].rIndex) &&
        ((Serial[port].uart->CR1 & USART_CR1_TCIE) == 0))
      Serial_Send_TX(port);

    // blocking! wait for buffer to become available
    while (((dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize) == dmaL1DataTX[port].rIndex) { }

    dmaL1DataTX[port].cache[dmaL1DataTX[port].wIndex] = *msg;                                 // copy character to cache
    dmaL1DataTX[port].wIndex = (dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize;  // update wIndex

    if ((*msg == '\n') && ((Serial[port].uart->CR1 & USART_CR1_TCIE) == 0))
      Serial_Send_TX(port);  // start DMA process if command is complete and DMA is not in progress already

    msg++;  // let the compiler optimize this, no need to do it manually!
  }
}

#else  // TX interrupt based serial writing

void Serial_Put(uint8_t port, const char * msg)
{
  while (*msg)
  {
    // blocking! wait for buffer to become available
    while (((dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize) == dmaL1DataTX[port].rIndex) { };

    dmaL1DataTX[port].cache[dmaL1DataTX[port].wIndex] = *msg++;
    dmaL1DataTX[port].wIndex = (dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize;

    Serial[port].uart->CR1 |= USART_CR1_TXEIE;  // set TXE interrupt bit to start the serial transfer
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

  #ifdef TX_DMA_WRITE  // TX DMA based serial writing
    if ((Serial[port].uart->SR & USART_SR_TC) != RESET)  // check for Transfer Complete (TC) interrupt
    {
      Serial[port].uart->SR &= ~USART_SR_TC;             // clear Transfer Complete (TC) bit

      // NOTE 1: use the serial TC, not the DMA TC because this only indicates DMA is done, peripheral might be still busy
      // NOTE 2: the TC interrupt is sometimes called while DMA is still active, so check NDTR status!
      //
      if (Serial[port].dma_streamTX->NDTR == 0)          // sending is complete
      {
        dmaL1DataTX[port].rIndex = (dmaL1DataTX[port].rIndex + dmaL1DataTX[port].flag) % dmaL1DataTX[port].cacheSize;
        dmaL1DataTX[port].flag = 0;

        if (dmaL1DataTX[port].wIndex != dmaL1DataTX[port].rIndex)  // is more data available?
          Serial_Send_TX(port);                                    // continue sending data
        else
          Serial[port].uart->CR1 &= ~USART_CR1_TCIE;               // disable Transfer Complete (TC) interrupt, nothing more to do
      }
      // else: more data is coming, wait for next Transfer Complete (TC) interrupt
    }
  #else  // TX interrupt based serial writing
    if ((Serial[port].uart->SR & USART_SR_TXE) != RESET)         // check for TXE interrupt
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
  #endif
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
