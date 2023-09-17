#include "Serial.h"
#include "includes.h"  // for infoHost

DMA_CIRCULAR_BUFFER dmaL1DataRX[_UART_CNT] = {0};  // DMA RX buffer
char * dmaL1DataTX[_UART_CNT] = {NULL};            // DMA TX buffer

// Config for USART Channel
typedef struct
{
  USART_TypeDef *uart;
  uint32_t dma_rcc;
  DMA_Channel_TypeDef *dma_channelRX;
  DMA_Channel_TypeDef *dma_channelTX;
} SERIAL_CFG;

static const SERIAL_CFG Serial[_UART_CNT] = {
  {USART1, RCC_AHBPeriph_DMA1, DMA1_Channel5, DMA1_Channel4},
  {USART2, RCC_AHBPeriph_DMA1, DMA1_Channel6, DMA1_Channel7},
  {USART3, RCC_AHBPeriph_DMA1, DMA1_Channel3, DMA1_Channel2},
  {UART4,  RCC_AHBPeriph_DMA2, DMA2_Channel3, DMA2_Channel5},
  //{UART5,  -1,  -1,  -1},  // UART5 don't support DMA
};

void Serial_DMA_Config(uint8_t port)
{
  const SERIAL_CFG * cfg = &Serial[port];

  RCC_AHBPeriphClockCmd(cfg->dma_rcc, ENABLE);  // DMA RCC EN

  cfg->dma_channelRX->CCR &= ~(1<<0);  // disable RX DMA
  cfg->dma_channelTX->CCR &= ~(1<<0);  // disable TX DMA

  cfg->uart->CR3 |= 3<<6;  // enable DMA RX-TX

  cfg->dma_channelRX->CPAR = (uint32_t)(&cfg->uart->DR);  // peripheral address RX (usart)
  cfg->dma_channelTX->CPAR = (uint32_t)(&cfg->uart->DR);  // peripheral address TX (usart)

  cfg->dma_channelRX->CMAR = (uint32_t)(dmaL1DataRX[port].cache);  // RX destination data (sram)
  cfg->dma_channelTX->CMAR = (uint32_t)(dmaL1DataTX[port]);        // TX source data (sram)

  cfg->dma_channelRX->CNDTR = dmaL1DataRX[port].cacheSize;  // RX buffer size
  cfg->dma_channelTX->CNDTR = 0;                            // TX buffer size

  cfg->dma_channelRX->CCR = 0x00000000;  // reset DMA RX CCR
  cfg->dma_channelTX->CCR = 0x00000000;  // reset DMA TX CCR

  if (port == SERIAL_PORT)
  { // primary serial port priority at highest level (TX higher than RX)
    cfg->dma_channelRX->CCR |= DMA_Priority_High;      // RX priority level: High
    cfg->dma_channelTX->CCR |= DMA_Priority_VeryHigh;  // TX priority level: Very high
  }
  else
  { // secondary serial ports priority at medium level
    cfg->dma_channelRX->CCR |= DMA_Priority_Medium;  // RX priority level: Medium
    cfg->dma_channelTX->CCR |= DMA_Priority_Medium;  // TX priority level: Medium
  }

  cfg->dma_channelRX->CCR &= ~(DMA_M2M_Enable);  // RX MEM2MEM disabled
  cfg->dma_channelTX->CCR &= ~(DMA_M2M_Enable);  // TX MEM2MEM disabled

  cfg->dma_channelRX->CCR &= ~(1<<4);  // data transfer direction: Peripheral-to-memory
  cfg->dma_channelTX->CCR |=   1<<4;   // data transfer direction: Memory-to-Peripheral

  cfg->dma_channelRX->CCR |=   DMA_Mode_Circular;   // RX circular mode enabled
  cfg->dma_channelTX->CCR &= ~(DMA_Mode_Circular);  // TX circular mode disabled

  cfg->dma_channelRX->CCR &= ~(DMA_PeripheralInc_Enable);  // RX peripheral no increment mode
  cfg->dma_channelTX->CCR &= ~(DMA_PeripheralInc_Enable);  // TX peripheral no increment mode

  cfg->dma_channelRX->CCR |= DMA_MemoryInc_Enable;  // RX memory increment mode
  cfg->dma_channelTX->CCR |= DMA_MemoryInc_Enable;  // TX memory increment mode

  cfg->dma_channelRX->CCR &= ~(3<<10);  // RX memory data size: 8 bit
  cfg->dma_channelTX->CCR &= ~(3<<10);  // TX memory data size: 8 bit

  cfg->dma_channelRX->CCR &= ~(3<<8);  // RX peripheral data size: 8 bit
  cfg->dma_channelTX->CCR &= ~(3<<8);  // TX peripheral data size: 8 bit

  cfg->dma_channelRX->CCR &= ~(DMA_IT_TE);  // RX transfer error interrupt disable
  cfg->dma_channelTX->CCR &= ~(DMA_IT_TE);  // TX transfer error interrupt disable

  cfg->dma_channelRX->CCR &= ~(DMA_IT_HT);  // RX half transfer interrupt disable
  cfg->dma_channelTX->CCR &= ~(DMA_IT_HT);  // TX half transfer interrupt disable

  cfg->dma_channelRX->CCR &= ~(DMA_IT_TC);  // RX transfer complete interrupt disable
  cfg->dma_channelTX->CCR &= ~(DMA_IT_TC);  // TX transfer complete interrupt disable

  cfg->dma_channelRX->CCR |= 1<<0;  // re-enable DMA RX
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
  while (!dmaL1DataTX[port]);  // malloc failed, blocking!

  UART_Config(port, baudrate, USART_IT_IDLE);  // IDLE interrupt
  Serial_DMA_Config(port);
}

void Serial_DeConfig(uint8_t port)
{
  Serial_ClearData(port);

  Serial[port].dma_channelRX->CCR &= ~(1<<0);  // Disable DMA RX
  Serial[port].dma_channelTX->CCR &= ~(1<<0);  // Disable DMA TX

  UART_DeConfig(port);
}

void Serial_Put(uint8_t port, const char *s) // send a zero terminated string to uart port by DMA
{
  // Waiting for previous TX to end, it is a blocking operation but
  // it gets effective usually only when emergency commands are sent.
  while(Serial[port].dma_channelTX->CNDTR != 0);

	Serial[port].dma_channelTX->CCR &= ~(1<<0);                          // disable TX DMA
  strncpy_no_pad(dmaL1DataTX[port], s, serialPort[port].cacheSizeTX);  // prepare TX data
  Serial[port].dma_channelTX->CNDTR = strlen(dmaL1DataTX[port]);       // set the number of bytes to be sent
	Serial[port].dma_channelTX->CCR |= 1<<0;                             // enable TX DMA (this will start the DMA TX process)
}

void USART_IRQHandler(uint8_t port)
{
  if ((Serial[port].uart->SR & USART_SR_IDLE) != RESET)
  {
    Serial[port].uart->SR;  // Clear idle line interrupt flag
    Serial[port].uart->DR;  // Clear RXNE pending bit

    dmaL1DataRX[port].wIndex = dmaL1DataRX[port].cacheSize - Serial[port].dma_channelRX->CNDTR;
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
