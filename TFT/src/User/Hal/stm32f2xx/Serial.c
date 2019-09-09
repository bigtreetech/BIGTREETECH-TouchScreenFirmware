#include "Serial.h"
#include "USART.h"

//Config for USART Channel
//USART1 RX DMA2 Channel4 Steam2/5
//USART2 RX DMA1 Channel4 Steam5
//USART3 RX DMA1 Channel4 Steam1
//UART4  RX DMA1 Channel4 Steam2
//UART5  RX DMA1 Channel4 Steam0
//USART6 RX DMA2 Channel5 Steam1/2
#if SERIAL_PORT == _USART1
  #define SERIAL_NUM              USART1
  #define SERIAL_DMA_RCC_AHB      RCC_AHB1Periph_DMA2
  #define SERIAL_DMA_STREAM       DMA2_Stream2
  #define SERIAL_DMA_CLEAR_FLAG() DMA2->LIFCR = (0x3F<<16)  //bit:16-21
  #define SERIAL_DMA_CHANNEL      4
#elif SERIAL_PORT == _USART2
  #define SERIAL_NUM              USART2
  #define SERIAL_DMA_RCC_AHB      RCC_AHB1Periph_DMA1
  #define SERIAL_DMA_STREAM       DMA1_Stream5
  #define SERIAL_DMA_CLEAR_FLAG() DMA1->HIFCR = (0xFC<<4)   //bit:6-11
  #define SERIAL_DMA_CHANNEL      4
#elif SERIAL_PORT == _USART3
  #define SERIAL_NUM              USART3
  #define SERIAL_DMA_RCC_AHB      RCC_AHB1Periph_DMA1
  #define SERIAL_DMA_STREAM       DMA1_Stream1
  #define SERIAL_DMA_CLEAR_FLAG() DMA1->LIFCR = (0xFC<<4)  //bit:6-11
  #define SERIAL_DMA_CHANNEL      4
#elif SERIAL_PORT == _UART4
  #define SERIAL_NUM              UART4
  #define SERIAL_DMA_RCC_AHB      RCC_AHB1Periph_DMA1
  #define SERIAL_DMA_STREAM       DMA1_Stream2
  #define SERIAL_DMA_CLEAR_FLAG() DMA1->LIFCR = (0x3F<<16)  //bit:16-21
  #define SERIAL_DMA_CHANNEL      4
#elif SERIAL_PORT == _UART5
  #define SERIAL_NUM              UART5
  #define SERIAL_DMA_RCC_AHB      RCC_AHB1Periph_DMA1
  #define SERIAL_DMA_STREAM       DMA1_Stream0
  #define SERIAL_DMA_CLEAR_FLAG() DMA1->LIFCR = (0x3F)  //bit:0-5
  #define SERIAL_DMA_CHANNEL      4
#elif SERIAL_PORT == _USART6
  #define SERIAL_NUM              USART6
  #define SERIAL_DMA_RCC_AHB      RCC_AHB1Periph_DMA2
  #define SERIAL_DMA_STREAM       DMA2_Stream1
  #define SERIAL_DMA_CLEAR_FLAG() DMA2->LIFCR = (0xFC<<4)  //bit:6-11
  #define SERIAL_DMA_CHANNEL      5
#endif

#define DMA_TRANS_LEN  ACK_MAX_SIZE

char *dma_mem_buf = ack_rev_buf;

void Serial_Config(u32 baud)
{
  RCC_AHB1PeriphClockCmd(SERIAL_DMA_RCC_AHB, ENABLE);  //DMA EN  
  USART_Config(SERIAL_PORT, baud, USART_IT_IDLE);  //IDLE interrupt

  SERIAL_NUM->CR3 |= 1<<6;  //DMA enable receiver
  
  SERIAL_DMA_STREAM->PAR = (u32)(&SERIAL_NUM->DR);
  SERIAL_DMA_STREAM->M0AR = (u32) dma_mem_buf;
  SERIAL_DMA_STREAM->NDTR = DMA_TRANS_LEN;
  
  SERIAL_DMA_STREAM->CR = SERIAL_DMA_CHANNEL << 25;
  SERIAL_DMA_STREAM->CR |= 3<<16;  // Priority level: Very high
  SERIAL_DMA_STREAM->CR |= 0<<13;  //Memory data size: 8
  SERIAL_DMA_STREAM->CR |= 0<<11;  //Peripheral data size: 8
  SERIAL_DMA_STREAM->CR |= 1<<10;  //Memory increment mode
  SERIAL_DMA_STREAM->CR |= 0<<9;   //Peripheral not increment mode
  SERIAL_DMA_STREAM->CR |= 0<<6;   //Data transfer direction: Peripheral-to-memory
  SERIAL_DMA_STREAM->CR |= 1<<0;   //enable DMA		
}

void Serial_DeConfig(void)
{
  USART_DeConfig(SERIAL_PORT);
}

/*

*/
void Serial_DMAReEnable(void)
{
  memset(dma_mem_buf,0,DMA_TRANS_LEN);
  SERIAL_DMA_STREAM->CR &= ~(1<<0);
  SERIAL_DMA_STREAM->NDTR = DMA_TRANS_LEN;  
  SERIAL_DMA_CLEAR_FLAG();        //clear ISR for rx complete
  SERIAL_DMA_STREAM->CR |= 1<<0;  //DMA			
}

void USART_IRQHandler(uint8_t port)
{
  u16 rx_len=0;

  if((SERIAL_NUM->SR & (1<<4))!=0)
  {
    SERIAL_NUM->SR = ~(1<<4);
    SERIAL_NUM->DR;

    rx_len = DMA_TRANS_LEN - SERIAL_DMA_STREAM->NDTR;

    if(dma_mem_buf[rx_len-1]=='\n')
    {
      infoHost.rx_ok=true;
    }
    else if(rx_len > DMA_TRANS_LEN-5)
    {
      infoHost.rx_ok=true;
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

void USART6_IRQHandler(void)
{
    USART_IRQHandler(_USART6);
}

void Serial_Puts(char *s )
{
  while (*s)
  {
    while((SERIAL_NUM->SR & USART_FLAG_TC) == (uint16_t)RESET);
    SERIAL_NUM->DR = ((u16)*s++ & (uint16_t)0x01FF);
  }
}

int fputc(int ch, FILE *f)
{      
	while((SERIAL_NUM->SR&0X40)==0);
    SERIAL_NUM->DR = (u8) ch;      
	return ch;
}
