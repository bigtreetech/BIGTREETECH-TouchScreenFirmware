#include "Serial.h"
#include "includes.h"  // for infoHost

DMA_CIRCULAR_BUFFER dmaL1DataRX[_UART_CNT] = {0};  // DMA RX buffer
char * dmaL1DataTX[_UART_CNT] = {NULL};            // DMA TX buffer

// Config for USART Channel
typedef struct
{
         USART_TypeDef *uart;
         uint32_t dma_rcc;
         uint8_t dma_channel;
         DMA_Stream_TypeDef *dma_streamRX;
         DMA_Stream_TypeDef *dma_streamTX;
} SERIAL_CFG;

static SERIAL_CFG Serial[_UART_CNT] = {
// USART   DMA1 or DMA2      Channel RX_STREAM    TX_STREAM
  {USART1, RCC_AHB1Periph_DMA2, 4, DMA2_Stream2, DMA2_Stream7},
  {USART2, RCC_AHB1Periph_DMA1, 4, DMA1_Stream5, DMA1_Stream6},
  {USART3, RCC_AHB1Periph_DMA1, 4, DMA1_Stream1, DMA1_Stream3},
  {UART4,  RCC_AHB1Periph_DMA1, 4, DMA1_Stream2, DMA1_Stream4},
  {UART5,  RCC_AHB1Periph_DMA1, 4, DMA1_Stream0, DMA1_Stream7},
  {USART6, RCC_AHB1Periph_DMA2, 5, DMA2_Stream1, DMA2_Stream6},
};

// Clear all DMA RX and TX interrupt flags for a serial port
void Serial_DMAClearITflags(uint8_t port)
{
// Channel to bits: Low  0: 0-5, 1: 6-11, 2: 16-21, 3: 22-27
// Channel to bits: High 4: 0-5, 5: 6-11, 6: 16-21, 7: 22-27

  switch(port) // DMA low/high interrupt Control Register (DMA_LIFCR/DMA_HIFCR)
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

  switch(port) // DMA low/high interrupt Control Register (DMA_LIFCR/DMA_HIFCR)
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
  switch(port) // DMA low/high interrupt Control Register (DMA_LIFCR/DMA_HIFCR)
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

  RCC_AHB1PeriphClockCmd(cfg->dma_rcc, ENABLE);  // DMA RCC enable

  cfg->dma_streamRX->CR &= ~(1<<0);  // disable RX DMA
  cfg->dma_streamTX->CR &= ~(1<<0);  // disable TX DMA

  Serial_DMAClearITflags(port);  // clear DMA RX-TX interrupt flags

  cfg->uart->CR3 |= 3<<6;  // DMA enable transmitter(DMAT) and receiver(DMAR)

  cfg->dma_streamRX->PAR  = (uint32_t)(&cfg->uart->DR);  // peripheral address RX (usart)
  cfg->dma_streamTX->PAR  = (uint32_t)(&cfg->uart->DR);  // peripheral address TX (usart)

  cfg->dma_streamRX->M0AR = (uint32_t)(dmaL1DataRX[port].cache);  // RX destination data (sram)
  cfg->dma_streamTX->M0AR = (uint32_t)(dmaL1DataTX[port]);        // TX source data (sram)

  cfg->dma_streamRX->NDTR = dmaL1DataRX[port].cacheSize;  // RX buffer size
  cfg->dma_streamTX->NDTR = 0;                            // TX buffer size

  cfg->dma_streamRX->CR = cfg->dma_channel << 25;  // RX channel selection, set to 0 all the other CR bits
  cfg->dma_streamTX->CR = cfg->dma_channel << 25;  // TX channel selection, set to 0 all the other CR bits

  if (port == SERIAL_PORT)
  { // primary serial port priority at highest level (TX higher than RX)
    cfg->dma_streamRX->CR |= DMA_Priority_High;      // RX priority level: High
    cfg->dma_streamTX->CR |= DMA_Priority_VeryHigh;  // TX priority level: Very high
  }
  else
  { // secondary serial ports priority at medium level
    cfg->dma_streamRX->CR &= DMA_Priority_Medium;  // RX priority level: Medium
    cfg->dma_streamTX->CR |= DMA_Priority_Medium;  // TX priority level: Medium
  }

  cfg->dma_streamRX->FCR &= ~(DMA_FIFOMode_Enable);  // enable DMA RX direct mode (no FIFO)
  cfg->dma_streamTX->FCR &= ~(DMA_FIFOMode_Enable);  // enable DMA TX direct mode (no FIFO)

  cfg->dma_streamRX->FCR &= ~(1<<7);  // enable DMA RX FIFO error interrupt disable
  cfg->dma_streamTX->FCR &= ~(1<<7);  // enable DMA TX FIFO error interrupt disable

  cfg->dma_streamRX->CR &= ~(1<<6);  // data transfer direction: Peripheral-to-memory
  cfg->dma_streamTX->CR |=   1<<6;   // data transfer direction: Memory-to-Peripheral

  cfg->dma_streamRX->CR &= ~(3<<13);  // memory data size: 8 bit
  cfg->dma_streamTX->CR &= ~(3<<13);  // memory data size: 8 bit

  cfg->dma_streamRX->CR &= ~(3<<11);  // peripheral data size: 8 bit
  cfg->dma_streamTX->CR &= ~(3<<11);  // peripheral data size: 8 bit

  cfg->dma_streamRX->CR |= DMA_MemoryInc_Enable;  // memory increment mode
  cfg->dma_streamTX->CR |= DMA_MemoryInc_Enable;  // memory increment mode

  cfg->dma_streamRX->CR &= ~(DMA_PeripheralInc_Enable);  // peripheral no increment mode
  cfg->dma_streamTX->CR &= ~(DMA_PeripheralInc_Enable);  // peripheral no increment mode

  cfg->dma_streamRX->CR |=   DMA_Mode_Circular;   // RX circular mode enabled
  cfg->dma_streamTX->CR &= ~(DMA_Mode_Circular);  // TX circular mode disabled

  cfg->dma_streamRX->CR &= ~(DMA_IT_TC);  // disable RX DMA Transfer Complete interrupt
  cfg->dma_streamTX->CR &= ~(DMA_IT_TC);  // disable TX DMA Transfer Complete interrupt

  cfg->dma_streamRX->CR &= ~(DMA_IT_HT);  // disable RX DMA Half Transfer interrupt
  cfg->dma_streamTX->CR &= ~(DMA_IT_HT);  // disable TX DMA Half Transfer interrupt

  cfg->dma_streamRX->CR &= ~(DMA_IT_TE);  // disable RX DMA transfer error interrupt
  cfg->dma_streamTX->CR &= ~(DMA_IT_TE);  // disable TX DMA transfer error interrupt

  cfg->dma_streamRX->CR &= ~(DMA_IT_DME);  // disable RX DMA direct mode error interrupt
  cfg->dma_streamTX->CR &= ~(DMA_IT_DME);  // disable TX DMA direct mode error interrupt

  cfg->dma_streamRX->CR |= 1<<0;  // re-enable RX DMA
}

void Serial_ClearData(uint8_t port)
{
  dmaL1DataRX[port].rIndex = dmaL1DataRX[port].wIndex = dmaL1DataRX[port].cacheSize = 0;
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
  while (!dmaL1DataRX[port].cache);  // malloc failed, blocking!
  dmaL1DataRX[port].rIndex = 0;
  dmaL1DataRX[port].wIndex = 0;

  dmaL1DataTX[port] = malloc(cacheSizeTX);
  while (!dmaL1DataTX[port]);  // malloc failed, blocking!

  UART_Config(port, baudrate, USART_IT_IDLE); // enable serial line IDLE interrupt
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

void Serial_Put(uint8_t port, const char *s) // send a zero terminated string to uart port by DMA
{
  // Waiting for previous TX to end, it is a blocking operation but
  // it gets effective usually only when emergency commands are sent.
  while(Serial[port].dma_streamTX->NDTR != 0);

  strncpy_no_pad(dmaL1DataTX[port], s, serialPort[port].cacheSizeTX);  // prepare TX data
  Serial[port].dma_streamTX->NDTR = strlen(dmaL1DataTX[port]);         // set the number of bytes to be sent

  // Before setting EN bit to '1' to start a new transfer, the event flags which
  // corresponds to the stream in DMA_LISR or DMA_HISR register must be cleared.
  Serial_DMAClearITflagsTX(port);         // clear DMA TX interrupt flags
  Serial[port].dma_streamTX->CR |= 1<<0;  // enable TX DMA (this will start the DMA TX process)
}

void USART_IRQHandler(uint8_t port)
{
  if ((Serial[port].uart->SR & USART_SR_IDLE) != RESET) // RX: check for serial Idle interrupt
  {
    Serial[port].uart->SR;  // Clear idle line interrupt flag
    Serial[port].uart->DR;  // Clear RXNE pending bit

    dmaL1DataRX[port].wIndex = dmaL1DataRX[port].cacheSize - Serial[port].dma_streamRX->NDTR;
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

void USART6_IRQHandler(void)
{
  USART_IRQHandler(_USART6);
}
