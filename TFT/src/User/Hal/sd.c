#include "variants.h"

#ifdef SD_SPI_SUPPORT
#include "sd.h"
#include "GPIO_Init.h"

u8 SD_Type = 0;  //SDCard type

/***************************************** SD SPI Interface ported by the underlying pattern***********************************************/
//#define SD_SPI        _SPI1
//#define SD_LOW_SPEED  7
//#define SD_HIGH_SPEED 1

//Read and write functions
u8 SD_SPI_Read_Write_Byte(u8 data)
{
  return SPI_Read_Write(SD_SPI, data);
}

//Chip Select
void SD_SPI_CS_Set(u8 level)
{
  GPIO_SetLevel(SD_CS_PIN, level);
}

#ifdef SD_CD_PIN
//SD_CD insert detect
void SD_CD_WP_Init(void)
{
  GPIO_InitSet(SD_CD_PIN, MGPIO_MODE_IPU, 0);
}
#endif

u8 SD_CD_Inserted(void)
{
#ifdef SD_CD_PIN
  return !GPIO_GetLevel(SD_CD_PIN);
#else
  return 0;
#endif
}

//initialization
void SD_SPI_Init(void)
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

//Low speed required during initialization
void SD_SetLowSpeed(void)
{
  SPI_Protocol_Init(SD_SPI, SD_LOW_SPEED);
}

//Use high speed when transferring data
void SD_SetHighSpeed(void)
{
  SPI_Protocol_Init(SD_SPI, SD_HIGH_SPEED);
}
/******************************************************************************************************************/

/************************************************************************************
**Deselect, release SPI bus
*************************************************************************************/
void SD_Cancel_CS(void)
{
  SD_SPI_CS_Set(1);
  SD_SPI_Read_Write_Byte(0xff);  //Provides additional 8 clocks
}

/************************************************************************************
**Select the SD card and wait for the card to be ready
**Return value: 0, success; 1, failure;
*************************************************************************************/
u8 SD_Select(void)
{
  SD_SPI_CS_Set(0);
  if (SD_Wait_Ready() == 0) return 0;  //Waiting for success
  SD_Cancel_CS();
  return 1;  //Wait for failure
}

/************************************************************************************
**Waiting for the card to be ready
** Return value: 0, ready; other, error code
**************************************************************************************/
u8 SD_Wait_Ready(void)
{
  u32 t = 0;

  do
  {
    if (SD_SPI_Read_Write_Byte(0XFF) == 0XFF) return 0;  //OK
    t++;
  } while (t<0XFFFFFF);  //wait
  return 1;
}

/************************************************************************************
** Waiting for SD card response
** Response: the response value to get
** Return value: 0, the response value was successfully obtained
** Other, failed to get response value
*************************************************************************************/
u8 SD_Get_Ack(u8 Response)
{
  uint16_t Count = 0xFFFF;  //Wait times

  while ((SD_SPI_Read_Write_Byte(0XFF) != Response) && Count) Count--;  //Waiting for an accurate response

  if (Count == 0)
    return SD_RESPONSE_FAILURE;  //Get response failed
  else
    return SD_RESPONSE_NO_ERROR;  //Respond correctly
}

/*************************************************************************************
** Read the contents of a packet from the SD card
** buf: data buffer area
** len: Length of data to be read.
** Return value: 0, success; others, failure;
****************************************************************************************/
u8 SD_RecvData(u8 * buf, uint16_t len)
{
  if (SD_Get_Ack(0xFE)) return 1;  //Wait for SD card to send back data start token 0xFE
  while (len--)  //Start receiving data
  {
    *buf = SD_SPI_Read_Write_Byte(0xFF);
    buf++;
  }
  //Here are 2 pseudo CRCs��dummy CRC��
  SD_SPI_Read_Write_Byte(0xFF);
  SD_SPI_Read_Write_Byte(0xFF);
  return 0;  //Read succeeded
}

/************************************************************************************
** Write the contents of a data packet to the SD card 512 bytes
** buf: data buffer area
** cmd: instruction
** Return value: 0, success; others, failure;
*************************************************************************************/
u8 SD_Send_Data(u8 * buf, u8 cmd)
{
  uint16_t t;

  if (SD_Wait_Ready()) return 1;  //Waiting for preparation to fail
  SD_SPI_Read_Write_Byte(cmd);
  if (cmd != 0XFD)  //Not an end instruction
  {
    for (t = 0; t < 512; t++) SD_SPI_Read_Write_Byte(buf[t]);

    SD_SPI_Read_Write_Byte(0xFF);  //Ignore crc
    SD_SPI_Read_Write_Byte(0xFF);
    t = SD_SPI_Read_Write_Byte(0xFF);  //Receive response
    if ((t & 0x1F) != 0x05) return 2;  //Response error
  }
  return 0;  //Write success
}

/*************************************************************************************
**Send a command to the SD card
** Enter: u8 cmd command
** u32 arg command parameters
** u8 crc crc check value
** Return value: Response returned by SD card
***************************************************************************************/
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
  u8 r1;
  u8 Retry = 0;

  SD_Cancel_CS();                //Cancel last selection
  if (SD_Select()) return 0XFF;  //Chip Select Failure
  //send
  SD_SPI_Read_Write_Byte(cmd | 0x40);  //Write commands separately
  SD_SPI_Read_Write_Byte(arg >> 24);
  SD_SPI_Read_Write_Byte(arg >> 16);
  SD_SPI_Read_Write_Byte(arg >> 8);
  SD_SPI_Read_Write_Byte(arg);
  SD_SPI_Read_Write_Byte(crc);
  if (cmd == CMD12) SD_SPI_Read_Write_Byte(0xff);  //Skip a stuff byte when stop reading
  //Waiting for a response, or exiting after a timeout
  Retry = 0X1F;
  do
  {
    r1 = SD_SPI_Read_Write_Byte(0xFF);
  } while ((r1 & 0X80) && Retry--);
  //Return status value
  return r1;
}

/*************************************************************************************
**Get CID information of SD card, including manufacturer information
**Input: u8 * cid_data (memory for CID, at least 16Byte)
**Return value: 0: NO_ERR
**       1��error
*************************************************************************************/
u8 SD_GetCID(u8 * cid_data)
{
  u8 r1;

  //Send CMD10 command, read CID
  r1 = SD_SendCmd(CMD10, 0, 0x01);
  if (r1 == 0x00)
  {
    r1 = SD_RecvData(cid_data, 16);  //Receive 16 bytes of data
  }
  SD_Cancel_CS();  //Cancel film selection
  if (r1)
    return 1;
  else
    return 0;
}

/*************************************************************************************
Get CSD information of SD card, including capacity and speed information
Input: u8 * cid_data (memory for CID, at least 16Byte)
Return value: 0: NO_ERR
1: error
*************************************************************************************/
u8 SD_GetCSD(u8 * csd_data)
{
  u8 r1;

  r1 = SD_SendCmd(CMD9, 0, 0x01);  //Send CMD9 command, read CSD
  if (r1 == 0)
  {
    r1 = SD_RecvData(csd_data, 16);  //Receive 16 bytes of data
  }
  SD_Cancel_CS();  //Cancel film selection
  if (r1)
    return 1;
  else
    return 0;
}

/************************************************************
**Get the total number of sectors of the SD card (the number of sectors)
**Return value: 0: Error in taking capacity
**Other: SD card capacity (number of sectors / 512 bytes)
**The number of bytes per sector must be 512 bytes, because if it is not 512 bytes, the initialization cannot pass.
*************************************************************/
u32 SD_Get_Sector_Count(void)
{
  u8 csd[16];
  u32 Capacity;
  u8 n;
  uint16_t csize;

  if (SD_GetCSD(csd) != 0) return 0;  //Get CSD information, if an error occurs during the period, return 0
  if ((csd[0] & 0xC0) == 0x40)        //V2.00 card, if it is SDHC card, calculate it as follows
  {
    csize = csd[9] + ((u16)csd[8] << 8) + 1;
    Capacity = (u32)csize << 10;  //Get the number of sectors
  }
  else  //V1.XX card
  {
    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
    csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
    Capacity= (u32)csize << (n - 9);  //Get the number of sectors
  }
  return Capacity;
}

/**********************************
Initialize SD card
***********************************/
u8 SD_Init(void)
{
  u8 r1;           // Store the return value of the SD card
  uint16_t retry;  // Used for timeout counting
  u8 buf[4];
  uint16_t i;

  SD_SPI_Init();  //Initialize IO
  SD_SetLowSpeed();
  for (i = 0; i < 10; i++) SD_SPI_Read_Write_Byte(0XFF);  //Send a minimum of 74 pulses
  retry = 20;
  do
  {
    r1 = SD_SendCmd(CMD0, 0, 0x95);  //Enter IDLE state
  } while ((r1 != 0X01) && retry--);
  SD_Type = 0;  // No card by default
  if (r1 == 0X01)
  {
    if (SD_SendCmd(CMD8, 0x1AA, 0x87) == 1)  //SD V2.0
    {
      for (i = 0; i < 4; i++) buf[i] = SD_SPI_Read_Write_Byte(0XFF);  //Get trailing return value of R7 resp
      if (buf[2] == 0X01 && buf[3] == 0XAA)  //Does the card support 2.7 ~ 3.6V
      {
        retry = 0XFFFE;
        do
        {
          SD_SendCmd(CMD55, 0, 0X01);  //Send CMD55
          r1 = SD_SendCmd(CMD41, 0x40000000, 0X01);  //Send CMD41
        } while (r1 && retry--);
        if (retry && SD_SendCmd(CMD58, 0, 0X01) == 0)  //Identification of SD2.0 card version started
        {
          for (i = 0; i < 4; i++) buf[i] = SD_SPI_Read_Write_Byte(0XFF);  //Get OCR value
          if (buf[0] & 0x40) SD_Type=SD_TYPE_V2HC;  //Check CCS
          else SD_Type = SD_TYPE_V2;
        }
      }
    }
    else  //SD V1.x/ MMC  V3
    {
      SD_SendCmd(CMD55, 0, 0X01);       //Send CMD55
      r1 = SD_SendCmd(CMD41, 0, 0X01);  //Send CMD41
      if (r1 <= 1)
      {
        SD_Type = SD_TYPE_V1;
        retry = 0XFFFE;
        do  //Waiting to exit IDLE mode
        {
          SD_SendCmd(CMD55, 0, 0X01);       //Send CMD55
          r1 = SD_SendCmd(CMD41, 0, 0X01);  //Send CMD41
        } while (r1 && retry--);
      }
      else  //MMCCard does not support CMD55 + CMD41 recognition
      {
        SD_Type = SD_TYPE_MMC;  //MMC V3
        retry = 0XFFFE;
        do  //Waiting to exit IDLE mode
        {
          r1 = SD_SendCmd(CMD1, 0, 0X01);  //Send CMD1
        } while (r1 && retry--);
      }
      if (retry == 0 || SD_SendCmd(CMD16, 512, 0X01) != 0)
        SD_Type = SD_TYPE_ERR;  //Wrong card
    }
  }
  SD_Cancel_CS();  //Cancel film selection
  SD_SetHighSpeed();
  if (SD_Type)
    return 0;
  else if (r1)
    return r1;
  return 0xaa;//��������
}

/*************************************************************************************
**Read SD card
** buf: data buffer area
** sector: sector
** cnt: Number of sectors
** Return value: 0, ok; other, failed.
*************************************************************************************/
u8 SD_ReadDisk(u8 * buf, u32 sector, u8 cnt)  //Read SD card, fatfs / usb calls x
{
  u8 r1;

  if (SD_Type != SD_TYPE_V2HC) sector <<= 9;  //Translate to byte address
  if (cnt == 1)
  {
    r1 = SD_SendCmd(CMD17, sector, 0X01);  //Read command
    if (r1 == 0)  //Instruction sent successfully
    {
      r1 = SD_RecvData(buf, 512);  //Receive 512 bytes
    }
  }
  else
  {
    r1 = SD_SendCmd(CMD18, sector, 0X01);  //Continuous read command
    do
    {
      r1 = SD_RecvData(buf, 512);  //Receive 512 bytes
      buf += 512;
    } while (--cnt && r1 == 0);
    SD_SendCmd(CMD12, 0, 0X01);  //Send stop command
  }
  SD_Cancel_CS();  //Cancel film selection
  return r1;
}

/*************************************************************************************
**Write SD card
**buf:Data buffer
**sector:Starting sector
**cnt:Number of sectors
**Return value: 0, ok; other, failed.
*************************************************************************************/
u8 SD_WriteDisk(u8 * buf, u32 sector, u8 cnt)  //Write SD card, fatfs / usb call
{
  u8 r1;

  if (SD_Type != SD_TYPE_V2HC) sector *= 512;  //Translate to byte address
  if (cnt == 1)
  {
    r1 = SD_SendCmd(CMD24, sector, 0X01);  //Read command
    if (r1 == 0)  //Instruction sent successfully
    {
      r1 = SD_Send_Data(buf, 0xFE);  //Write 512 bytes
    }
  }
  else
  {
    if (SD_Type != SD_TYPE_MMC)
    {
      SD_SendCmd(CMD55, 0, 0X01);
      SD_SendCmd(CMD23, cnt, 0X01);  //Send instruction
    }
    r1 = SD_SendCmd(CMD25, sector, 0X01);  //Continuous read command
    if (r1 == 0)
    {
      do
      {
        r1 = SD_Send_Data(buf, 0xFC);  //Receive 512 bytes
        buf += 512;
      } while (--cnt && r1 == 0);
      r1 = SD_Send_Data(0, 0xFD);  // Receive 512 bytes
    }
  }
  SD_Cancel_CS();  //Cancel film selection
  return r1;
}

#endif
