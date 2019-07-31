#include "Serial.h"
#include "USART.h"

//Config for USART Channel
#if SERIAL_PORT == _USART1
  #define SERIAL_NUM          USART1
  #define SERIAL_DMA_RCC_AHB  RCC_AHBPeriph_DMA1
  #define SERIAL_DMA_CHANNEL  DMA1_Channel5
#elif SERIAL_PORT == _USART2
  #define SERIAL_NUM          USART2
  #define SERIAL_DMA_RCC_AHB  RCC_AHBPeriph_DMA1
  #define SERIAL_DMA_CHANNEL  DMA1_Channel6
#elif SERIAL_PORT == _USART3
  #define SERIAL_NUM          USART3
  #define SERIAL_DMA_RCC_AHB  RCC_AHBPeriph_DMA1
  #define SERIAL_DMA_CHANNEL  DMA1_Channel3
#elif SERIAL_PORT == _UART4
  #define SERIAL_NUM          UART4
  #define SERIAL_DMA_RCC_AHB  RCC_AHBPeriph_DMA2
  #define SERIAL_DMA_CHANNEL  DMA2_Channel3
#elif SERIAL_PORT == _UART5
  #error "UART5 don't support DMA"
#endif

#define DMA_TRANS_LEN  ACK_MAX_SIZE

char *dma_mem_buf = ack_rev_buf;

void Serial_Config(u32 baud)
{
  RCC_AHBPeriphClockCmd(SERIAL_DMA_RCC_AHB, ENABLE);  //DMA EN  
  USART_Config(SERIAL_PORT, baud, USART_IT_IDLE);  //IDLE interrupt

  SERIAL_NUM->CR3 |= 1<<6;  //DMA enable receiver
  
  SERIAL_DMA_CHANNEL->CPAR = (u32)(&SERIAL_NUM->DR);
  SERIAL_DMA_CHANNEL->CMAR = (u32) dma_mem_buf;
  SERIAL_DMA_CHANNEL->CNDTR = DMA_TRANS_LEN;
  SERIAL_DMA_CHANNEL->CCR = 0X00000000;
  SERIAL_DMA_CHANNEL->CCR |= 3<<12;   //Channel priority level
  SERIAL_DMA_CHANNEL->CCR |= 1<<7;    //Memory increment mode
  SERIAL_DMA_CHANNEL->CCR |= 1<<0;    //DMA		
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
  SERIAL_DMA_CHANNEL->CCR &= ~(1<<0);
  SERIAL_DMA_CHANNEL->CNDTR = DMA_TRANS_LEN;  
  SERIAL_DMA_CHANNEL->CCR |= 1<<0;    //DMA			
}

void USART_IRQHandler(uint8_t port)
{
  u16 rx_len=0;

  if((SERIAL_NUM->SR & (1<<4))!=0)
  {
    SERIAL_NUM->SR = ~(1<<4);
    SERIAL_NUM->DR;

    rx_len = DMA_TRANS_LEN - SERIAL_DMA_CHANNEL->CNDTR;

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
