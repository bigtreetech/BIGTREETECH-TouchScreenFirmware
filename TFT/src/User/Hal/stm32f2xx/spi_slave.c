#include "spi_slave.h"
#include "spi.h"
#include "GPIO_Init.h"

//TODO:
//now support SPI2 and PB12 CS only
//more compatibility changes are needed
//Config for SPI Channel
#if ST7920_SPI == _SPI1
  #define ST7920_SPI_NUM          SPI1
#elif ST7920_SPI == _SPI2
  #define ST7920_SPI_NUM          SPI2
#elif ST7920_SPI == _SPI3
  #define ST7920_SPI_NUM          SPI3
#endif

//#define _SPI_SLAVE_IRQ(n)  n##_IRQHandler
//#define SPI_SLAVE_IRQ  _SPI_SLAVE_IRQ(W25QXX_SPI_NUM)

SPI_QUEUE SPISlave;

void SPI_ReEnable(u8 mode)
{
  ST7920_SPI_NUM->CR1 = (0<<15)  //0:˫��˫�� 1:����˫��
                      | (0<<14)  //����˫��ʱʹ�ã�0:ֻ�� 1:ֻ����
                      | (0<<13)  //1:����Ӳ��CRC
                      | (0<<12)  //0:��һ�����͵�ֵ���Է��ͻ����� 1:��һ�����͵�ֵ���Է���CRC�Ĵ�����
                      | (0<<11)  //0:8λ����֡ 1:16λ����֡
                      | (1<<10)  //0:ȫ˫�� 1:ֻ����
                      | (1<<9)   //0:Ӳ��NSS 1:���NSS
                      | (0<<8)   //SSMλΪ��1��ʱ�����塣��������NSS�ϵĵ�ƽ
                      | (0<<7)   //0:MSB 1:LSB
                      | (7<<3)   //bit3-5   000�� fPCLK/2 001�� fPCLK/4 010�� fPCLK/8 011�� fPCLK/16
                                 //         100�� fPCLK/32 101�� fPCLK/64 110�� fPCLK/128 111�� fPCLK/256
                      | (0<<2)   //0:���豸 1:���豸
                      | (mode<<1)   //CPOL
                      | (mode<<0);  //CPHA
            
  ST7920_SPI_NUM->CR2 |= 1<<6;  //�����ж�ʹ�� SPI_I2S_IT_RXNE
}

void SPI_Slave(void) 
{
  NVIC_InitTypeDef   NVIC_InitStructure;

  SPI_GPIO_Init(ST7920_SPI);  
  GPIO_InitSet(PB12, MGPIO_MODE_IPU, 0);  //CS
  
  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
  SPI_ReEnable(1);
}

void SPI_SlaveDeInit(void)
{  
  NVIC_InitTypeDef   NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  SPI_I2S_DeInit(ST7920_SPI_NUM);
}

void SPI2_IRQHandler(void) 
{ 
  SPISlave.data[SPISlave.wIndex] =  ST7920_SPI_NUM->DR;
  SPISlave.wIndex = (SPISlave.wIndex + 1) % SPI_SLAVE_MAX;
} 

/* �ⲿ�ж����� */
void SPI_Slave_CS_Config(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure; 

  /* ��GPIOA_0���ж������� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ�� 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);//PB12 ���ӵ��ж���12

  /* �����ж���0λ�ⲿ�½����ж� */
  EXTI_InitStructure.EXTI_Line = EXTI_Line12;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);


  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//��ռ���ȼ�2�� 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure); 
}


/* �ⲿ�ж� */
void EXTI15_10_IRQHandler(void)
{
  if((GPIOB->IDR & (1<<12)) != 0)
  {
    SPI_ReEnable(!!(GPIOB->IDR & (1<<13))); //����Ӧ spi mode0/mode3
    ST7920_SPI_NUM->CR1 |= (1<<6);
  }
  else
  {
    RCC->APB1RSTR |= 1<<14;	//��λSPI1
    RCC->APB1RSTR &= ~(1<<14);
  }
/* ����ж�״̬�Ĵ��� */
  EXTI->PR = 1<<12;
}
