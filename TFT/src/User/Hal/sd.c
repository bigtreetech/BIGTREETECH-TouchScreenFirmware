#include "sd.h"

#ifdef SD_SPI_SUPPORT

#include "GPIO_Init.h"
#include "spi.h"

// SDCard type definition
#define SD_TYPE_ERR  0X00
#define SD_TYPE_MMC  0X01
#define SD_TYPE_V1   0X02
#define SD_TYPE_V2   0X04
#define SD_TYPE_V2HC 0X06

// SDCard instruction table
#define CMD0  0   // card reset
#define CMD1  1
#define CMD8  8   // command 8, SEND_IF_COND
#define CMD9  9   // command 9 to read CSD data
#define CMD10 10  // command 10, read CID data
#define CMD12 12  // command 12, stop data transmission
#define CMD16 16  // command 16, set sector size should return 0x00
#define CMD17 17  // command 17, read sector
#define CMD18 18  // command 18, read Multi sector
#define CMD23 23  // command 23, set N blocks to be erased before multi-sector write
#define CMD24 24  // command 24, write sector
#define CMD25 25  // command 25, write multiple sectors
#define CMD41 41  // command 41, should return 0x00
#define CMD55 55  // command 55, should return 0x01
#define CMD58 58  // command 58, read OCR information
#define CMD59 59  // command 59, enable / disable CRC, should return 0x00

// data write response word meaning
#define SD_DATA_OK          0x05
#define SD_DATA_CRC_ERROR   0x0B
#define SD_DATA_WRITE_ERROR 0x0D
#define SD_DATA_OTHER_ERROR 0xFF

// SD Card Response Marker
#define SD_RESPONSE_NO_ERROR    0x00  // response error
#define SD_IN_IDLE_STATE        0x01  // idle state
#define SD_ERASE_RESET          0x02  // erase reset
#define SD_ILLEGAL_COMMAND      0x04
#define SD_COM_CRC_ERROR        0x08
#define SD_ERASE_SEQUENCE_ERROR 0x10
#define SD_ADDRESS_ERROR        0x20
#define SD_PARAMETER_ERROR      0x40
#define SD_RESPONSE_FAILURE     0xFF

static uint8_t SD_Type = 0;  // SDCard type

/**************************** SD SPI Interface ported by the underlying pattern ****************************/
//#define SD_SPI        _SPI1
//#define SD_LOW_SPEED  7
//#define SD_HIGH_SPEED 1

// read and write functions
static uint8_t SD_SPI_Read_Write_Byte(uint8_t data)
{
  return SPI_Read_Write(SD_SPI, data);
}

// Chip Select
static void SD_SPI_CS_Set(uint8_t level)
{
  GPIO_SetLevel(SD_CS_PIN, level);
}

#ifdef SD_CD_PIN

// SD_CD insert detect
static inline void SD_CD_WP_Init(void)
{
  GPIO_InitSet(SD_CD_PIN, MGPIO_MODE_IPU, 0);
}

#endif

uint8_t SD_CD_Inserted(void)
{
  #ifdef SD_CD_PIN
    return !GPIO_GetLevel(SD_CD_PIN);
  #else
    return 0;
  #endif
}

// initialization
static inline void SD_SPI_Init(void)
{
  #ifdef SD_CD_PIN
    SD_CD_WP_Init();
  #endif
  GPIO_InitSet(SD_CS_PIN, MGPIO_MODE_OUT_PP, 0);
  SPI_Config(SD_SPI);
  SD_SPI_CS_Set(1);
}

void SD_DeInit(void)
{
  GPIO_InitSet(SD_CS_PIN, MGPIO_MODE_IPN, 0);
  SPI_DeConfig(SD_SPI);
}

// low speed required during initialization
static inline void SD_SetLowSpeed(void)
{
  SPI_Protocol_Init(SD_SPI, SD_LOW_SPEED);
}

// use high speed when transferring data
static inline void SD_SetHighSpeed(void)
{
  SPI_Protocol_Init(SD_SPI, SD_HIGH_SPEED);
}

/****************************************************************************************************************/

/************************************************************************************
* deselect, release SPI bus
*************************************************************************************/
static void SD_Cancel_CS(void)
{
  SD_SPI_CS_Set(1);
  SD_SPI_Read_Write_Byte(0xff);  // provides additional 8 clocks
}

/************************************************************************************
* select the SD card and wait for the card to be ready
* return value: 0, success; 1, failure;
*************************************************************************************/
static inline uint8_t SD_Select(void)
{
  SD_SPI_CS_Set(0);

  if (SD_Wait_Ready() == 0)
    return 0;  // waiting for success

  SD_Cancel_CS();

  return 1;  // wait for failure
}

/************************************************************************************
* waiting for the card to be ready
* return value: 0, ready; other, error code
*************************************************************************************/
uint8_t SD_Wait_Ready(void)
{
  uint32_t t = 0;

  do
  {
    if (SD_SPI_Read_Write_Byte(0XFF) == 0XFF)
      return 0;  // OK

    t++;
  } while (t<0XFFFFFF);  // wait

  return 1;
}

/************************************************************************************
* waiting for SD card response
* response: the response value to get
* return value: 0, the response value was successfully obtained
* other, failed to get response value
*************************************************************************************/
uint8_t SD_Get_Ack(uint8_t Response)
{
  uint16_t Count = 0xFFFF;  // wait times

  while ((SD_SPI_Read_Write_Byte(0XFF) != Response) && Count) Count--;  // waiting for an accurate response

  if (Count == 0)
    return SD_RESPONSE_FAILURE;  // get response failed
  else
    return SD_RESPONSE_NO_ERROR;  // respond correctly
}

/************************************************************************************
* read the contents of a packet from the SD card
* buf: data buffer area
* len: Length of data to be read.
* return value: 0, success; others, failure;
*************************************************************************************/
static uint8_t SD_RecvData(uint8_t * buf, uint16_t len)
{
  if (SD_Get_Ack(0xFE))
    return 1;  // wait for SD card to send back data start token 0xFE

  while (len--)  // start receiving data
  {
    *buf = SD_SPI_Read_Write_Byte(0xFF);
    buf++;
  }

  // here are 2 pseudo CRCs dummy CRC
  SD_SPI_Read_Write_Byte(0xFF);
  SD_SPI_Read_Write_Byte(0xFF);

  return 0;  // read succeeded
}

/************************************************************************************
* write the contents of a data packet to the SD card 512 bytes
* buf: data buffer area
* cmd: instruction
* return value: 0, success; others, failure;
*************************************************************************************/
static uint8_t SD_Send_Data(uint8_t * buf, uint8_t cmd)
{
  uint16_t t;

  if (SD_Wait_Ready())
    return 1;  // waiting for preparation to fail

  SD_SPI_Read_Write_Byte(cmd);

  if (cmd != 0XFD)  // not an end instruction
  {
    for (t = 0; t < 512; t++) SD_SPI_Read_Write_Byte(buf[t]);

    SD_SPI_Read_Write_Byte(0xFF);  // ignore crc
    SD_SPI_Read_Write_Byte(0xFF);
    t = SD_SPI_Read_Write_Byte(0xFF);  // receive response

    if ((t & 0x1F) != 0x05)
      return 2;  // response error
  }

  return 0;  // write success
}

/************************************************************************************
* send a command to the SD card
* enter: uint8_t cmd command
* uint32_t arg command parameters
* uint8_t crc crc check value
* return value: Response returned by SD card
*************************************************************************************/
static uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
  uint8_t r1;
  uint8_t Retry = 0;

  SD_Cancel_CS();  // cancel last selection

  if (SD_Select())
    return 0XFF;  // Chip Select Failure

  // send
  SD_SPI_Read_Write_Byte(cmd | 0x40);  // write commands separately
  SD_SPI_Read_Write_Byte(arg >> 24);
  SD_SPI_Read_Write_Byte(arg >> 16);
  SD_SPI_Read_Write_Byte(arg >> 8);
  SD_SPI_Read_Write_Byte(arg);
  SD_SPI_Read_Write_Byte(crc);

  if (cmd == CMD12)
    SD_SPI_Read_Write_Byte(0xff);  // skip a stuff byte when stop reading

  // waiting for a response, or exiting after a timeout
  Retry = 0X1F;

  do
  {
    r1 = SD_SPI_Read_Write_Byte(0xFF);
  } while ((r1 & 0X80) && Retry--);

  // return status value
  return r1;
}

/************************************************************************************
* get CID information of SD card, including manufacturer information
* input: uint8_t * cid_data (memory for CID, at least 16Byte)
* return value: 0: NO_ERR
*               1: error
*************************************************************************************/
uint8_t SD_GetCID(uint8_t * cid_data)
{
  uint8_t r1;

  // send CMD10 command, read CID
  r1 = SD_SendCmd(CMD10, 0, 0x01);

  if (r1 == 0x00)
    r1 = SD_RecvData(cid_data, 16);  // receive 16 bytes of data

  SD_Cancel_CS();  // cancel film selection

  if (r1)
    return 1;
  else
    return 0;
}

/************************************************************************************
* get CSD information of SD card, including capacity and speed information
* input: uint8_t * cid_data (memory for CID, at least 16Byte)
* return value: 0: NO_ERR
*               1: error
*************************************************************************************/
uint8_t SD_GetCSD(uint8_t * csd_data)
{
  uint8_t r1;

  r1 = SD_SendCmd(CMD9, 0, 0x01);  // send CMD9 command, read CSD

  if (r1 == 0)
    r1 = SD_RecvData(csd_data, 16);  // receive 16 bytes of data

  SD_Cancel_CS();  // cancel film selection

  if (r1)
    return 1;
  else
    return 0;
}

/************************************************************************************
* get the total number of sectors of the SD card (the number of sectors)
* return value: 0: Error in taking capacity
* other: SD card capacity (number of sectors / 512 bytes)
* the number of bytes per sector must be 512 bytes, because if it is not 512 bytes, the initialization cannot pass
*************************************************************************************/
uint32_t SD_Get_Sector_Count(void)
{
  uint8_t csd[16];
  uint32_t Capacity;
  uint8_t n;
  uint16_t csize;

  if (SD_GetCSD(csd) != 0)
    return 0;  // get CSD information, if an error occurs during the period, return 0

  if ((csd[0] & 0xC0) == 0x40)  // V2.00 card, if it is SDHC card, calculate it as follows
  {
    csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
    Capacity = (uint32_t)csize << 10;  // get the number of sectors
  }
  else  // V1.XX card
  {
    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
    csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
    Capacity= (uint32_t)csize << (n - 9);  // get the number of sectors
  }

  return Capacity;
}

/************************************************************************************
* initialize SD card
*************************************************************************************/
uint8_t SD_Init(void)
{
  uint8_t r1;      // store the return value of the SD card
  uint16_t retry;  // used for timeout counting
  uint8_t buf[4];
  uint16_t i;

  SD_SPI_Init();  // initialize IO
  SD_SetLowSpeed();

  for (i = 0; i < 10; i++) SD_SPI_Read_Write_Byte(0XFF);  // send a minimum of 74 pulses

  retry = 20;

  do
  {
    r1 = SD_SendCmd(CMD0, 0, 0x95);  // enter IDLE state
  } while ((r1 != 0X01) && retry--);

  SD_Type = 0;  // no card by default

  if (r1 == 0X01)
  {
    if (SD_SendCmd(CMD8, 0x1AA, 0x87) == 1)  // SD V2.0
    {
      for (i = 0; i < 4; i++) buf[i] = SD_SPI_Read_Write_Byte(0XFF);  // get trailing return value of R7 resp

      if (buf[2] == 0X01 && buf[3] == 0XAA)  // does the card support 2.7 ~ 3.6V
      {
        retry = 0XFFFE;

        do
        {
          SD_SendCmd(CMD55, 0, 0X01);                // send CMD55
          r1 = SD_SendCmd(CMD41, 0x40000000, 0X01);  // send CMD41
        } while (r1 && retry--);

        if (retry && SD_SendCmd(CMD58, 0, 0X01) == 0)  // identification of SD2.0 card version started
        {
          for (i = 0; i < 4; i++) buf[i] = SD_SPI_Read_Write_Byte(0XFF);  // get OCR value

          if (buf[0] & 0x40)  // check CCS
            SD_Type=SD_TYPE_V2HC;
          else
            SD_Type = SD_TYPE_V2;
        }
      }
    }
    else  // SD V1.x/ MMC  V3
    {
      SD_SendCmd(CMD55, 0, 0X01);       // send CMD55
      r1 = SD_SendCmd(CMD41, 0, 0X01);  // send CMD41

      if (r1 <= 1)
      {
        SD_Type = SD_TYPE_V1;
        retry = 0XFFFE;

        do  // waiting to exit IDLE mode
        {
          SD_SendCmd(CMD55, 0, 0X01);       // send CMD55
          r1 = SD_SendCmd(CMD41, 0, 0X01);  // send CMD41
        } while (r1 && retry--);
      }
      else  // MMCCard does not support CMD55 + CMD41 recognition
      {
        SD_Type = SD_TYPE_MMC;  // MMC V3
        retry = 0XFFFE;

        do  // waiting to exit IDLE mode
        {
          r1 = SD_SendCmd(CMD1, 0, 0X01);  // send CMD1
        } while (r1 && retry--);
      }

      if (retry == 0 || SD_SendCmd(CMD16, 512, 0X01) != 0)
        SD_Type = SD_TYPE_ERR;  // wrong card
    }
  }

  SD_Cancel_CS();  // cancel film selection
  SD_SetHighSpeed();

  if (SD_Type)
    return 0;
  else if (r1)
    return r1;

  return 0xaa;
}

/************************************************************************************
* read SD card
* buf: data buffer area
* sector: sector
* cnt: Number of sectors
* return value: 0, ok; other, failed.
*************************************************************************************/
uint8_t SD_ReadDisk(uint8_t * buf, uint32_t sector, uint8_t cnt)  // read SD card, fatfs / usb calls x
{
  uint8_t r1;

  if (SD_Type != SD_TYPE_V2HC)
    sector <<= 9;  // translate to byte address

  if (cnt == 1)
  {
    r1 = SD_SendCmd(CMD17, sector, 0X01);  // read command

    if (r1 == 0)                   // instruction sent successfully
      r1 = SD_RecvData(buf, 512);  // receive 512 bytes
  }
  else
  {
    r1 = SD_SendCmd(CMD18, sector, 0X01);  // continuous read command

    do
    {
      r1 = SD_RecvData(buf, 512);  // receive 512 bytes
      buf += 512;
    } while (--cnt && r1 == 0);

    SD_SendCmd(CMD12, 0, 0X01);  // send stop command
  }

  SD_Cancel_CS();  // cancel film selection

  return r1;
}

/************************************************************************************
* write SD card
* buf:Data buffer
* sector:Starting sector
* cnt:Number of sectors
* return value: 0, ok; other, failed.
*************************************************************************************/
uint8_t SD_WriteDisk(uint8_t * buf, uint32_t sector, uint8_t cnt)  // write SD card, fatfs / usb call
{
  uint8_t r1;

  if (SD_Type != SD_TYPE_V2HC)
    sector *= 512;  // translate to byte address

  if (cnt == 1)
  {
    r1 = SD_SendCmd(CMD24, sector, 0X01);  // read command

    if (r1 == 0)                     // instruction sent successfully
      r1 = SD_Send_Data(buf, 0xFE);  // write 512 bytes
  }
  else
  {
    if (SD_Type != SD_TYPE_MMC)
    {
      SD_SendCmd(CMD55, 0, 0X01);
      SD_SendCmd(CMD23, cnt, 0X01);  // send instruction
    }

    r1 = SD_SendCmd(CMD25, sector, 0X01);  // continuous read command

    if (r1 == 0)
    {
      do
      {
        r1 = SD_Send_Data(buf, 0xFC);  // receive 512 bytes
        buf += 512;
      } while (--cnt && r1 == 0);

      r1 = SD_Send_Data(0, 0xFD);  // receive 512 bytes
    }
  }

  SD_Cancel_CS();  // cancel film selection

  return r1;
}

#endif  // SD_SPI_SUPPORT
