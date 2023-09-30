#include "Serial.h"
#include "includes.h"

// set this line to "true" to enable IDLE Line interrupt. It is no more needed, so always set this macro to "false"
#define IDLE_LINE_IT false

DMA_CIRCULAR_BUFFER dmaL1DataRX[_UART_CNT] = {0};  // DMA / interrupt RX buffer
DMA_CIRCULAR_BUFFER dmaL1DataTX[_UART_CNT] = {0};  // DMA / interrupt TX buffer

// config for USART DMA channels
const SERIAL_CFG Serial[_UART_CNT] = {
#ifdef TX_DMA_WRITE
// USART   RCU DMAx  DMAx  RX Channel TX Channel
  {USART0, RCU_DMA0, DMA0, 4,         3},
  {USART1, RCU_DMA0, DMA0, 5,         6},
  {USART2, RCU_DMA0, DMA0, 2,         1},
  {UART3,  RCU_DMA1, DMA1, 2,         4},
  {UART4,  RCU_DMA1, DMA1, 1,         3},
  {USART5, RCU_DMA1, DMA1, 5,         6},
#else
// USART   RCU DMAx  DMAx  RX Channel
  {USART0, RCU_DMA0, DMA0, 4},
  {USART1, RCU_DMA0, DMA0, 5},
  {USART2, RCU_DMA0, DMA0, 2},
  {UART3,  RCU_DMA1, DMA1, 2},
  {UART4,  RCU_DMA1, DMA1, 1},
  {USART5, RCU_DMA1, DMA1, 5},
#endif
};

// disable RX DMA and clear all interrupt flags for a serial port
void Serial_DMA_DisableAndClearFlagsRX(uint8_t port)
{
  DMA_CHCTL(Serial[port].dma_stream, Serial[port].dma_channelRX) &= ~(1<<0);  // disable RX DMA

  // table 12.5.2, 4 bits per channel, shift = channel * 4

  switch (port)
  {
    case _USART1:
      DMA_INTC(DMA0) = (0x0F << 16);  // DMA0 channel 4
      break;

    case _USART2:
      DMA_INTC(DMA0) = (0x0F << 20);  // DMA0 channel 5
      break;

    case _USART3:
      DMA_INTC(DMA0) = (0x0F << 8);   // DMA0 channel 2
      break;

    case _UART4:
      DMA_INTC(DMA1) = (0x0F << 8);   // DMA1 channel 2
      break;

    case _UART5:
      DMA_INTC(DMA1) = (0x0F << 4);   // DMA1 channel 1
      break;

    case _USART6:
      DMA_INTC(DMA1) = (0x0F << 20);  // DMA1 channel 5
      break;
  }
}

#ifdef TX_DMA_WRITE

// disable TX DMA and clear all interrupt flags for a serial port
void Serial_DMA_DisableAndClearFlagsTX(uint8_t port)
{
  DMA_CHCTL(Serial[port].dma_stream, Serial[port].dma_channelTX) &= ~(1<<0);  // disable TX DMA

  // table 12.5.2, 4 bits per channel, shift = channel * 4

  switch (port)
  {
    case _USART1:
      DMA_INTC(DMA0) = (0x0F << 12);  // DMA0 channel 3
      break;

    case _USART2:
      DMA_INTC(DMA0) = (0x0F << 24);  // DMA0 channel 6
      break;

    case _USART3:
      DMA_INTC(DMA0) = (0x0F << 4);   // DMA0 channel 1
      break;

    case _UART4:
      DMA_INTC(DMA1) = (0x0F << 16);  // DMA1 channel 4
      break;

    case _UART5:
      DMA_INTC(DMA1) = (0x0F << 12);  // DMA1 channel 3
      break;

    case _USART6:
      DMA_INTC(DMA1) = (0x0F << 24);  // DMA1 channel 6
      break;
  }
}

#endif

void Serial_DMA_Config(uint8_t port)
{
  const SERIAL_CFG * cfg = &Serial[port];

  rcu_periph_clock_enable(cfg->dma_rcc);                        // enable DMA clock

  Serial_DMA_DisableAndClearFlagsRX(port);                      // RX disable DMA and clear all interrupt flags

  DMA_CHPADDR(cfg->dma_stream, cfg->dma_channelRX) = (uint32_t)(&USART_DATA(cfg->uart));     // RX peripheral address (usart)
  DMA_CHMADDR(cfg->dma_stream, cfg->dma_channelRX) = (uint32_t)(dmaL1DataRX[port].cache);    // RX destination address (memory)
  DMA_CHCNT(cfg->dma_stream, cfg->dma_channelRX) = (uint32_t)(dmaL1DataRX[port].cacheSize);  // RX buffer size

  DMA_CHCTL(cfg->dma_stream, cfg->dma_channelRX) = 0;           // RX clear control register

  // primary serial port priority at highest level (TX higher than RX)
  if (port == SERIAL_PORT)
    DMA_CHCTL(cfg->dma_stream, cfg->dma_channelRX) |= (2<<12);  // RX priority level: High
  else
    DMA_CHCTL(cfg->dma_stream, cfg->dma_channelRX) |= (0<<12);  // RX priority level: Low

//DMA_CHCTL(cfg->dma_stream, cfg->dma_channelRX) |= (0<<10);    // RX memory data size: 8 bit
//DMA_CHCTL(cfg->dma_stream, cfg->dma_channelRX) |= (0<<8);     // RX peripheral data size: 8 bit
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channelRX) |= (1<<7);     // RX memory increment mode
//DMA_CHCTL(cfg->dma_stream, cfg->dma_channelRX) |= (0<<6);     // RX peripheral no increment mode
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channelRX) |= (1<<5);     // RX circular mode enabled
//DMA_CHCTL(cfg->dma_stream, cfg->dma_channelRX) |= (0<<4);     // RX data transfer direction: Peripheral-to-memory

#ifdef TX_DMA_WRITE  // TX DMA based serial writing
  Serial_DMA_DisableAndClearFlagsTX(port);                      // TX disable DMA and clear all interrupt flags

  DMA_CHPADDR(cfg->dma_stream, cfg->dma_channelTX) = (uint32_t)(&USART_DATA(cfg->uart));  // TX peripheral address (usart)

  DMA_CHCTL(cfg->dma_stream, cfg->dma_channelTX) = 0;           // TX clear control register

  // primary serial port priority at highest level (TX higher than RX)
  if (port == SERIAL_PORT)
    DMA_CHCTL(cfg->dma_stream, cfg->dma_channelTX) |= (3<<12);  // TX priority level: Very high
  else
    DMA_CHCTL(cfg->dma_stream, cfg->dma_channelTX) |= (1<<12);  // TX priority level: Medium

//DMA_CHCTL(cfg->dma_stream, cfg->dma_channelTX) |= (0<<10);    // TX memory data size: 8 bit
//DMA_CHCTL(cfg->dma_stream, cfg->dma_channelTX) |= (0<<8);     // TX peripheral data size: 8 bit
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channelTX) |= (1<<7);     // TX memory increment mode
//DMA_CHCTL(cfg->dma_stream, cfg->dma_channelTX) |= (0<<6);     // TX peripheral no increment mode
//DMA_CHCTL(cfg->dma_stream, cfg->dma_channelTX) |= (0<<5);     // TX circular mode disabled
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channelTX) |= (1<<4);     // TX data transfer direction: Memory-to-Peripheral

  USART_CTL2(cfg->uart) |= (1<<7);                              // enable DMA transmitter (DMAT)
//DMA_CHCTL(cfg->dma_stream, cfg->dma_channelTX) |= (1<<0);     // TX enable DMA, done later when needed
#endif

  USART_CTL2(cfg->uart) |= (1<<6);                              // enable DMA receiver (DMAR)
  DMA_CHCTL(cfg->dma_stream, cfg->dma_channelRX) |= (1<<0);     // RX enable DMA
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

void Serial_Config(uint8_t port, uint32_t cacheSizeRX, uint32_t cacheSizeTX, uint32_t baudrate)
{
  Serial_ClearData(port);

  dmaL1DataRX[port].cacheSize = cacheSizeRX;
  dmaL1DataRX[port].cache = malloc(cacheSizeRX);
  while (!dmaL1DataRX[port].cache);                           // RX malloc failed, blocking!

  dmaL1DataTX[port].cacheSize = cacheSizeTX;
  dmaL1DataTX[port].cache = malloc(cacheSizeTX);
  while (!dmaL1DataTX[port].cache);                           // TX malloc failed, blocking!

  UART_Config(port, baudrate, USART_INT_IDLE, IDLE_LINE_IT);  // configure serial line with or without IDLE Line interrupt

  Serial_DMA_Config(port);
}

void Serial_DeConfig(uint8_t port)
{
  Serial_ClearData(port);

  Serial_DMA_DisableAndClearFlagsRX(port);  // disable RX DMA and clear all interrupt flags

#ifdef TX_DMA_WRITE
  Serial_DMA_DisableAndClearFlagsTX(port);  // disable TX DMA and clear all interrupt flags
#endif

  UART_DeConfig(port);
}

#ifdef TX_DMA_WRITE  // TX DMA based serial writing

// DMA serial write support function
void Serial_Send_TX(uint8_t port)
{
  // setup DMA transfer, and wait for serial Transfer Complete (TX) interrupt in ISR
  if (dmaL1DataTX[port].wIndex >= dmaL1DataTX[port].rIndex)
    dmaL1DataTX[port].flag = dmaL1DataTX[port].wIndex - dmaL1DataTX[port].rIndex;
  else  // split transfer into 2 parts
    dmaL1DataTX[port].flag = dmaL1DataTX[port].cacheSize - dmaL1DataTX[port].rIndex;

  Serial_DMA_DisableAndClearFlagsTX(port);   // disable TX DMA and clear all interrupt flags

  DMA_CHMADDR(Serial[port].dma_stream, Serial[port].dma_channelTX) = (uint32_t)(&dmaL1DataTX[port].cache[dmaL1DataTX[port].rIndex]);  // TX data start address
  DMA_CHCNT(Serial[port].dma_stream, Serial[port].dma_channelTX) = dmaL1DataTX[port].flag;                                            // number of bytes to transfer

  USART_CTL0(Serial[port].uart) |= USART_CTL0_TCIE;                          // enable Transfer Complete (TC) serial interrupt
  DMA_CHCTL(Serial[port].dma_stream, Serial[port].dma_channelTX) |= (1<<0);  // enable TX DMA
}

void Serial_Put(uint8_t port, const char * msg)
{
  while (*msg)
  {
    // setup TX DMA, if no '\n' yet, but buffer is full AND DMA is not in progress already (waiting for Transfer Complete interrupt)
    if ((((dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize) == dmaL1DataTX[port].rIndex) &&
        ((USART_CTL0(Serial[port].uart) & USART_CTL0_TCIE) == 0))
      Serial_Send_TX(port);

    // blocking! wait for buffer to become available
    while (((dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize) == dmaL1DataTX[port].rIndex) { }

    dmaL1DataTX[port].cache[dmaL1DataTX[port].wIndex] = *msg;                                 // copy character to cache
    dmaL1DataTX[port].wIndex = (dmaL1DataTX[port].wIndex + 1) % dmaL1DataTX[port].cacheSize;  // update wIndex

    if ((*msg == '\n') && ((USART_CTL0(Serial[port].uart) & USART_CTL0_TCIE) == 0))
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

    USART_CTL0(Serial[port].uart) |= USART_CTL0_TBEIE;  // set TBE interrupt bit to start the serial transfer
  }
}

#endif

// ISR, serial interrupt handler
void USART_IRQHandler(uint8_t port)
{
#if IDLE_LINE_IT == true  // IDLE Line interrupt
  if ((USART_STAT0(Serial[port].uart) & USART_STAT0_IDLEF) != RESET)  // check for IDLE Line interrupt
  {
    USART_STAT0(Serial[port].uart);                                   // clear IDLE Line bit
    USART_DATA(Serial[port].uart);

    dmaL1DataRX[port].wIndex = dmaL1DataRX[port].cacheSize - DMA_CHCNT(Serial[port].dma_stream, Serial[port].dma_channelRX);
  }
#endif

#ifdef TX_DMA_WRITE  // TX DMA based serial writing
  if ((USART_STAT0(Serial[port].uart) & USART_STAT0_TC) != RESET)  // check for Transfer Complete (TC) interrupt
  {
    USART_STAT0(Serial[port].uart) &= ~USART_STAT0_TC;             // clear Transfer Complete (TC) bit

    // NOTE 1: use the serial TC, not the DMA TC because this only indicates DMA is done, peripheral might be still busy
    // NOTE 2: the TC interrupt is sometimes called while DMA is still active, so check NDTR status!
    //
    if (DMA_CHCNT(Serial[port].dma_stream, Serial[port].dma_channelTX) == 0)  // sending is complete
    {
      dmaL1DataTX[port].rIndex = (dmaL1DataTX[port].rIndex + dmaL1DataTX[port].flag) % dmaL1DataTX[port].cacheSize;
      dmaL1DataTX[port].flag = 0;

      if (dmaL1DataTX[port].wIndex != dmaL1DataTX[port].rIndex)  // is more data available?
        Serial_Send_TX(port);                                    // continue sending data
      else
        USART_CTL0(Serial[port].uart) &= ~USART_CTL0_TCIE;       // disable Transfer Complete (TC) interrupt, nothing more to do
    }
    // else: more data is coming, wait for next Transfer Complete (TC) interrupt
  }
#else  // TX interrupt based serial writing
  if ((USART_STAT0(Serial[port].uart) & USART_STAT0_TBE) != RESET)  // check for TBE interrupt
  {
    if (dmaL1DataTX[port].rIndex == dmaL1DataTX[port].wIndex)       // no more data?
    {
      USART_CTL0(Serial[port].uart) &= ~USART_CTL0_TBEIE;           // disable TBE interrupt
    }
    else
    {
      USART_DATA(Serial[port].uart) = (uint8_t)dmaL1DataTX[port].cache[dmaL1DataTX[port].rIndex];  // write next available character

      dmaL1DataTX[port].rIndex = (dmaL1DataTX[port].rIndex + 1) % dmaL1DataTX[port].cacheSize;     // increase reading index
    }
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
