/**
 * For MKS_TFT35_V1_0 I2C FLASH MEMORY EEPROM support
 * HARD CODED FOR I2C1!!!
 */
#include "i2c_eeprom.h"
#include "includes.h"  // for I2C_EEPROM, GUI_DispString, Delay_us etc.
#include "GPIO_Init.h"

#ifdef I2C_EEPROM

#define EEPROM_OK         0
#define EEPROM_FAIL       1
#define EEPROM_TIMEOUT    3

#define AT24C01             127  // PAGE_SIZE  8 bytes
#define AT24C02             255  // PAGE_SIZE  8 bytes
#define AT24C04             511  // PAGE_SIZE 16 bytes
#define AT24C08            1023  // PAGE_SIZE 16 bytes
#define AT24C16            2047  // PAGE_SIZE 16 bytes
#define AT24C32            4095
#define AT24C64            8191
#define AT24C128          16383
#define AT24C256          32767

//#define AT24CXX_TYPE    EEPROM_TYPE
#define AT24CXX_ADDRESS   EEPROM_ADDRESS  // address
#define AT24CXX_PAGE_SIZE 16              // 8 bytes

// timeout for write based on max read/write which is
// AT24CXX_PAGE_SIZE bytes: 8 bits * AT24CXX_PAGE_SIZE / I2C_SPEED
//#define EEPROM_READ_TIMEOUT  ((uint32_t)(1000))
//#define EEPROM_WRITE_TIMEOUT ((uint32_t)(1000))

static inline void I2C1_init(uint32_t I2C_baudrate)
{
  I2C_InitTypeDef I2C_InitStructure;

  I2C_DeInit(I2C1);  // deinit I2C1

  // configure I2C1
  I2C_InitStructure.I2C_ClockSpeed          = I2C_baudrate;                  // 400kHz
  I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;                  // I2C mode
  I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;               // 50% duty cycle --> standard
  I2C_InitStructure.I2C_OwnAddress1         = 0x00;                          // own address, not relevant in master mode
  I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;                // enable acknowledge when reading (can be changed later on)
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;  // set address length to 7 bit addresses
  I2C_Init(I2C1, &I2C_InitStructure);                                        // init I2Cx
  I2C_Cmd(I2C1, ENABLE);                                                     // enable I2C1
  I2C_StretchClockCmd(I2C1, ENABLE);
}

static inline void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);     // enable GPIO clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);      // enable I2C clock

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);   // SCL, Pin B6
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);   // SDA, Pin B7

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;  // use PB6 and PB9
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             // set pins to alternate function
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;        // set GPIO speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;            // set output to open drain, only pulled low, don't drive high
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;             // enable pull up resistors
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  // connect I2Cx pins to Alternative Function I2C
}

/*===============================================================================================================*/
/* I2C_Write_Byte */
/*===============================================================================================================*/
static void I2C_Write_Byte(I2C_TypeDef * I2Cx, uint8_t reg_addr, uint8_t reg_data, uint8_t dev_Addr)
{
  while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));  // while the bus is busy

  I2C_GenerateSTART(I2Cx, ENABLE);  // send I2C1 START condition
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

  GPIO_SetLevel(PB4, 0);  // pin 6 of XS13 Auto Power Off for MKS_TFT35_V1_0

  I2C_Send7bitAddress(I2Cx, dev_Addr, I2C_Direction_Transmitter);  // send Device slave Address for write
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  GPIO_SetLevel(PA8, 0);  // WIFI_RST for MKS_TFT35_V1_0

  I2C_SendData(I2Cx, reg_addr);  // send the Device internal register address
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  I2C_SendData(I2Cx, reg_data);  // send I2C1 EEPROM data
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  I2C_AcknowledgeConfig(I2Cx, DISABLE);  // disable Acknowledgement
  Delay_us(1);                           // generate 12.285uS delay as required

  I2C_GenerateSTOP(I2Cx, ENABLE);  // send I2C1 STOP Condition
  Delay_us(1);                     // generate 12.285uS delay as required

  I2C_AcknowledgeConfig(I2Cx, ENABLE);  // enable Acknowledgement to be ready for another reception
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

  GPIO_SetLevel(PA8, 1);  // WIFI_RST for MKS_TFT35_V1_0
  GPIO_SetLevel(PB4, 1);  // pin 6 of XS13 Auto Power Off for MKS_TFT35_V1_0
}

/*=============================================================================================*/
/* I2C_Read_Byte */
/*=============================================================================================*/
static uint8_t I2C_Read_Byte(I2C_TypeDef * I2Cx, uint8_t reg_addr, uint8_t dev_Addr)
{
  char Red_reg;

  /*------------------------------ start dummy write ----------------------------------------*/
  while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));  // while the bus is busy

  I2C_GenerateSTART(I2Cx, ENABLE);  // send I2C1 START condition
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_Send7bitAddress(I2Cx, dev_Addr, I2C_Direction_Transmitter);  // send Device slave Address for write
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  I2C_SendData(I2Cx, reg_addr);  // send the Device internal register address
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  /*------------------------------- dummy write end -----------------------------------------*/

  /*------------------------------ start random read ----------------------------------------*/

  I2C_GenerateSTART(I2Cx, ENABLE);  // send START condition a second time called repeated start
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_Send7bitAddress(I2Cx, (dev_Addr | 0x01), I2C_Direction_Receiver);  // send Device slave Address for read
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

  // test on I2C1 EV7 and clear it. Check for BUSY, MSL and RXNE flags = (EV7)
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

  Red_reg = I2C_ReceiveData(I2Cx);

  I2C_AcknowledgeConfig(I2Cx, DISABLE);   // disable Acknowledgement
  Delay_us(1);                            // generate 12.285uS delay as required
  I2C_GenerateSTOP(I2Cx, ENABLE);         // send I2C1 STOP Condition
  Delay_us(1);                            // generate 12.285uS delay as required
  //I2C_AcknowledgeConfig(I2C1, ENABLE);  // enable Acknowledgement to be ready for another reception

  return Red_reg;  // return data which is to be read from device
}

static inline uint8_t I2C_Write(I2C_TypeDef * I2Cx, const uint8_t * buf, uint8_t reg, uint32_t nbyte, uint8_t SlaveAddress)
{
  // enable Error IT (used in all modes: DMA, Polling and Interrupts
  //I2Cx->CR2 |= I2C_IT_ERR;

  if (nbyte)
  {
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

    // intiate Start Sequence
    I2C_GenerateSTART(I2Cx, ENABLE);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    // send Address  EV5
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // EV6 - write reg
    I2C_SendData(I2Cx, reg);
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF));

    // write first byte EV8_1
    if (nbyte > 16)
      GUI_DispString(200, 0, (uint8_t *)"large byte write");

    I2C_SendData(I2Cx, *buf++);

    while (--nbyte)
    { // wait on BTF
      while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF));

      I2C_SendData(I2Cx, *buf++);
    }

    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF));

    I2C_GenerateSTOP(I2Cx, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
  }

  return 0;
}

static inline void EEPROM_ReadBuffer(uint8_t * pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
  //uint8_t I2C_Read_Byte(I2C_TypeDef * I2Cx, uint8_t reg_addr, uint8_t dev_Addr)
  *pBuffer = I2C_Read_Byte(I2C1, ReadAddr, AT24CXX_ADDRESS);

  for (uint16_t i = 0; i < NumByteToRead; i++)
  {
    *pBuffer = I2C_Read_Byte(I2C1, ReadAddr + i, AT24CXX_ADDRESS);
    pBuffer++;
    // NumByteToRead;
  }
}

static inline uint8_t I2Cx_WriteMultiple(uint8_t Addr, uint16_t Reg, uint8_t * Buffer, uint16_t Length)
{
  uint8_t status = EEPROM_OK;

  I2C_Write(I2C1, Buffer, Reg, Length, Addr);

  // check the communication status
  if (status != EEPROM_OK)
  {
    // re-Initiaize the I2C Bus
    //I2Cx_Error(Addr);
  }

  return status;
}

static uint8_t EEPROM_PageWrite(uint16_t MemAddress, uint8_t * pBuffer, uint32_t BufferSize)
{
  uint8_t status = EEPROM_OK;

  status = I2Cx_WriteMultiple((AT24CXX_ADDRESS + ((MemAddress / 256) << 1)), MemAddress, pBuffer, BufferSize);
  Delay_us(3000);

  return status;
}

static inline uint8_t EEPROM_WriteData(uint16_t MemAddress, uint8_t * pBuffer, uint32_t BufferSize)
{
  uint8_t NumOfPage = 0;
  uint8_t NumOfSingle = 0;
  uint8_t Addr = 0;
  uint8_t count = 0;
  uint8_t err = EEPROM_OK;

  Addr = MemAddress % AT24CXX_PAGE_SIZE;
  count = AT24CXX_PAGE_SIZE - Addr;
  NumOfPage =  BufferSize / AT24CXX_PAGE_SIZE;
  NumOfSingle = BufferSize % AT24CXX_PAGE_SIZE;

  // if WriteAddr is I2C_PageSize aligned
  if (Addr == 0)
  {
    // if NumByteToWrite < I2C_PageSize
    if (NumOfPage == 0)
    {
      return (EEPROM_PageWrite(MemAddress, pBuffer, BufferSize));
    }
    // if NumByteToWrite > I2C_PageSiz
    else
    {
      while (NumOfPage--)
      {
        err = EEPROM_PageWrite(MemAddress, pBuffer, AT24CXX_PAGE_SIZE);

        if (err != EEPROM_OK)
          return err;

        MemAddress +=  AT24CXX_PAGE_SIZE;
        pBuffer += AT24CXX_PAGE_SIZE;
      }

      if (NumOfSingle != 0)
        return (EEPROM_PageWrite(MemAddress, pBuffer, NumOfSingle));
    }
  }
  // if WriteAddr is not I2C_PageSize aligned
  else
  { // if NumByteToWrite < I2C_PageSize
    if (BufferSize <= count)
      return (EEPROM_PageWrite(MemAddress, pBuffer, NumOfSingle));

    else
    { // if NumByteToWrite > I2C_PageSize
      BufferSize -= count;
      NumOfPage   = BufferSize / AT24CXX_PAGE_SIZE;
      NumOfSingle = BufferSize % AT24CXX_PAGE_SIZE;

      err = EEPROM_PageWrite(MemAddress, pBuffer, count);

      if (err != EEPROM_OK)
        return err;

      MemAddress += count;
      pBuffer += count;

      while (NumOfPage--)
      {
        err = EEPROM_PageWrite(MemAddress, pBuffer, AT24CXX_PAGE_SIZE);

        if (err != EEPROM_OK)
          return err;

        MemAddress += AT24CXX_PAGE_SIZE;
        pBuffer    += AT24CXX_PAGE_SIZE;
      }

      if (NumOfSingle != 0)
        return (EEPROM_PageWrite(MemAddress, pBuffer, NumOfSingle));
    }
  }

  return err;
}

void i2C_Init_EEPROM(void)
{
  I2C_GPIO_Config();
  I2C1_init(I2C_SPEED);
}

void EEPROM_FlashRead(uint8_t * data, uint16_t len)
{
  EEPROM_ReadBuffer(data, 0, (uint16_t)len);
}

void EEPROM_FlashWrite(uint8_t * data, uint16_t len)
{
  EEPROM_WriteData(0x0, data, (uint16_t)len);
}

#endif  // I2C_EEPROM
