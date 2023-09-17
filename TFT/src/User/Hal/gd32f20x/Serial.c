#include "Serial.h"
#include "includes.h"  // for infoHost


DMA_CIRCULAR_BUFFER dmaL1DataRX[_UART_CNT] = {0};  // DMA RX buffer
char * dmaL1DataTX[_UART_CNT] = {NULL};            // DMA TX buffer

// Config for USART Channel
typedef struct
{
  uint32_t uart;
  rcu_periph_enum dma_rcc;
  uint32_t dma_stream;
  uint8_t dma_channelRX;
  uint8_t dma_channelTX;
} SERIAL_CFG;

static const SERIAL_CFG Serial[_UART_CNT] = {
  {USART0, RCU_DMA0, DMA0, 4, 3},
  {USART1, RCU_DMA0, DMA0, 5, 6},
  {USART2, RCU_DMA0, DMA0, 2, 1},
  {UART3,  RCU_DMA1, DMA1, 2, 4},
  //{UART4,  RCU_DMA1, DMA1, 1, 0},
  //{USART5, RCU_DMA1, DMA1, 5, 6},
};

// Clear all DMA RX interrupt flags for a serial port
void Serial_DMAClearFlagRX(uint8_t port)
{
  DMA_INTC(Serial[port].dma_stream) = 0x0F << (4 * Serial[port].dma_channelRX);
}

// Clear all DMA TX interrupt flags for a serial port
void Serial_DMAClearFlagTX(uint8_t port)
{
  DMA_INTC(Serial[port].dma_stream) = 0x0F << (4 * Serial[port].dma_channelTX);
}

// Clear all DMA RX and TX interrupt flags for a serial port
void Serial_DMAClearFlag(uint8_t port)
{
  DMA_INTC(Serial[port].dma_stream) = (0x0F << (4 * Serial[port].dma_channelRX)) | (0x0F << (4 * Serial[port].dma_channelTX));
}

void Serial_DMA_Config(uint8_t port)
{
  const SERIAL_CFG * serial = &Serial[port];
  rcu_periph_clock_enable(serial->dma_rcc);

  DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) &= ~(1<<0);   // Disable DMA RX
  DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) &= ~(1<<0);   // Disable DMA TX

  Serial_DMAClearFlag(port);
  USART_CTL2(serial->uart) |= 3<<6;  // enable uart DMA RX-TX

  DMA_CHPADDR(serial->dma_stream, serial->dma_channelRX) = (uint32_t)(&USART_DATA(serial->uart));  // DMA RX peripheral address
  DMA_CHPADDR(serial->dma_stream, serial->dma_channelTX) = (uint32_t)(&USART_DATA(serial->uart));  // DMA TX peripheral address

  DMA_CHMADDR(serial->dma_stream, serial->dma_channelRX) = (uint32_t)(dmaL1DataRX[port].cache);  // DMA RX memory address
  DMA_CHMADDR(serial->dma_stream, serial->dma_channelTX) = (uint32_t)(dmaL1DataTX[port]);        // DMA TX memory address

  DMA_CHCNT(serial->dma_stream, serial->dma_channelRX) = dmaL1DataRX[port].cacheSize;  // DMA RX data count
  DMA_CHCNT(serial->dma_stream, serial->dma_channelTX) = 0;                            // DMA TX data count

  DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) = 0x00;  // reset DMA RX control register
  DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) = 0x00;  // reset DMA TX control register

  if (port == SERIAL_PORT)
  { // primary serial port priority at highest level (TX higher than RX)
    DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) |= DMA_PRIORITY_HIGH;        // Priority level: High
    DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) |= DMA_PRIORITY_ULTRA_HIGH;  // Priority level: Ultra high
  }
  else
  { // secondary serial ports priority at medium level
    DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) |= DMA_PRIORITY_MEDIUM;  // Priority level: Medium
    DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) |= DMA_PRIORITY_MEDIUM;  // Priority level: Medium
  }

  DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) &= ~(3<<10);  // Memory data size: 8
  DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) &= ~(3<<10);  // Memory data size: 8

  DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) &= ~(3<<8);  // Peripheral data size: 8
  DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) &= ~(3<<8);  // Peripheral data size: 8

  DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) |= 1<<7;  // Memory increment mode
  DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) |= 1<<7;  // Memory increment mode

  DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) &= ~(1<<6);  // Peripheral not increment mode
  DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) &= ~(1<<6);  // Peripheral not increment mode

  DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) |=   1<<5;   // Circular mode enabled
  DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) &= ~(1<<5);  // Circular mode disabled

  DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) &= ~(1<<4);  // Data transfer direction: Peripheral-to-memory
  DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) |=   1<<4;   // Data transfer direction: Memory-to-peripheral

  DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) &= ~(1<<3);  // Channel error interrupt disabled
  DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) &= ~(1<<3);  // Channel error interrupt disabled

  DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) &= ~(1<<2);  // Channel half transfer finish interrupt disabled
  DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) &= ~(1<<2);  // Channel half transfer finish interrupt disabled

  DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) &= ~(1<<1);  // Channel full transfer finish interrupt disabled
  DMA_CHCTL(serial->dma_stream, serial->dma_channelTX) &= ~(1<<1);  // Channel full transfer finish interrupt disabled

  DMA_CHCTL(serial->dma_stream, serial->dma_channelRX) |= 1<<0;  // Re-enable DMA RX
}

void Serial_ClearData(uint8_t port)
{
  dmaL1DataRX[port].wIndex = dmaL1DataRX[port].rIndex = dmaL1DataRX[port].flag = dmaL1DataRX[port].cacheSize = 0;

  if (dmaL1DataRX[port].cache != NULL)
  {
    free(dmaL1DataRX[port].cache);
    dmaL1DataRX[port].cache = NULL;
  }

  if (dmaL1DataTX[port] != NULL)
  {
    free(dmaL1DataTX[port]);
    dmaL1DataTX[port] = NULL;
  }
}

void Serial_Config(uint8_t port, uint16_t cacheSizeRX, uint16_t cacheSizeTX, uint32_t baudrate)
{
  Serial_ClearData(port);

  dmaL1DataRX[port].cacheSize = cacheSizeRX;
  dmaL1DataRX[port].cache = malloc(cacheSizeRX);
  while (!dmaL1DataRX[port].cache);  // malloc failed
  dmaL1DataRX[port].rIndex = 0;
  dmaL1DataRX[port].wIndex = 0;

  dmaL1DataTX[port] = malloc(cacheSizeTX);
  while (!dmaL1DataTX[port]);  // malloc failed

  UART_Config(port, baudrate, USART_INT_IDLE);  // IDLE interrupt
  Serial_DMA_Config(port);
}

void Serial_DeConfig(uint8_t port)
{
  Serial_ClearData(port);

  DMA_CHCTL(Serial[port].dma_stream, Serial[port].dma_channelRX) &= ~(1<<0);  // Disable DMA RX
  DMA_CHCTL(Serial[port].dma_stream, Serial[port].dma_channelTX) &= ~(1<<0);  // Disable DMA TX

  Serial_DMAClearFlag(port);
  UART_DeConfig(port);
}


void Serial_Put(uint8_t port, const char *s) // send a zero terminated string to uart port by DMA
{
  // Waiting for previous TX to end, it is a blocking operation but
  // it gets effective usually only when emergency commands are sent.
  while(DMA_CHCNT(Serial[port].dma_stream, Serial[port].dma_channelTX) != 0);

  DMA_CHCTL(Serial[port].dma_stream, Serial[port].dma_channelTX) &= ~(1<<0);                   // disable DMA TX
  strncpy_no_pad(dmaL1DataTX[port], s, serialPort[port].cacheSizeTX);                          // prepare TX data
  DMA_CHCNT(Serial[port].dma_stream, Serial[port].dma_channelTX) = strlen(dmaL1DataTX[port]);  // set the number of bytes to be sent
  DMA_CHCTL(Serial[port].dma_stream, Serial[port].dma_channelTX) |= 1<<0;                      // enable DMA TX (this will start the DMA TX process)
}

void USART_IRQHandler(uint8_t port)
{
  if ((USART_STAT0(Serial[port].uart) & USART_STAT0_IDLEF) != RESET) // RX: check for serial Idle interrupt
  {
    USART_STAT0(Serial[port].uart);  // Clear idle line interrupt flag
    USART_DATA(Serial[port].uart);   // Clear RXNE pending bit

    dmaL1DataRX[port].wIndex = dmaL1DataRX[port].cacheSize - DMA_CHCNT(Serial[port].dma_stream, Serial[port].dma_channelRX);
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
