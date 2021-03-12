#include "w25qxx.h"

/*************************** W25Qxx SPI Interface ported by the underlying pattern ***************************/
//#define W25Qxx_SPI     _SPI3
//#define W25Qxx_SPEED   0
//#define W25Qxx_CS_PIN PA4

const uint8_t cap_ID[14] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x43, 0x4B, 0x00, 0x01};
const uint32_t flash_size[14] = {KB(64), KB(128), KB(256), KB(512), MB(1), MB(2), MB(4), MB(8), MB(16), MB(32), MB(8), MB(8), KB(256), KB(512)};

//Chip Select
void W25Qxx_SPI_CS_Set(u8 level)
{
  GPIO_SetLevel(W25Qxx_CS_PIN, level);
}

//Read and write functions
uint8_t W25Qxx_SPI_Read_Write_Byte(uint8_t data)
{
  return SPI_Read_Write(W25Qxx_SPI,data);
}

//initialization
void W25Qxx_Init(void)
{
  GPIO_InitSet(W25Qxx_CS_PIN, MGPIO_MODE_OUT_PP, 0);
  SPI_Config(W25Qxx_SPI);
  SPI_Protocol_Init(W25Qxx_SPI, W25Qxx_SPEED);
  W25Qxx_SPI_CS_Set(1);
}
/*************************************************************************************/

// Write enable
void W25Qxx_WriteEnable(void)
{
  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_WRITE_ENABLE);
  W25Qxx_SPI_CS_Set(1);
}

//Waiting for W25Qxx to be idle
void W25Qxx_WaitForWriteEnd(void)
{
  uint8_t flashstatus = 0;

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_REGISTER1);
  do
  {
    flashstatus = W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
  }
  while ((flashstatus & 0x01) == SET);
  W25Qxx_SPI_CS_Set(1);
}

//Write by page
void W25Qxx_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  W25Qxx_WriteEnable();
  W25Qxx_SPI_CS_Set(0);

  W25Qxx_SPI_Read_Write_Byte(CMD_PAGE_PROGRAM);
  W25Qxx_SPI_Read_Write_Byte((WriteAddr & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((WriteAddr & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(WriteAddr & 0xFF);

  while (NumByteToWrite--)
  {
    W25Qxx_SPI_Read_Write_Byte(*pBuffer);
    pBuffer++;
  }

  W25Qxx_SPI_CS_Set(1);
  W25Qxx_WaitForWriteEnd();
}

//Send buffer data to FLASH
void W25Qxx_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % W25QXX_SPI_PAGESIZE;              //Represents which address in a page starts to write data
  count = W25QXX_SPI_PAGESIZE - Addr;                  //How much data can be written on this page
  NumOfPage =  NumByteToWrite / W25QXX_SPI_PAGESIZE;   //Represents how many pages can be written to the length of the data to be stored
  NumOfSingle = NumByteToWrite % W25QXX_SPI_PAGESIZE;  //Represents the length of data that can be written in addition to the entire page of data

  if (Addr == 0)  /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0)  /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      W25Qxx_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else  /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      while (NumOfPage--)
      {
        W25Qxx_WritePage(pBuffer, WriteAddr, W25QXX_SPI_PAGESIZE);
        WriteAddr +=  W25QXX_SPI_PAGESIZE;
        pBuffer += W25QXX_SPI_PAGESIZE;
      }
      W25Qxx_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else  /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0)  /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count)  /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        W25Qxx_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        W25Qxx_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        W25Qxx_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else  /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / W25QXX_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % W25QXX_SPI_PAGESIZE;

      W25Qxx_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        W25Qxx_WritePage(pBuffer, WriteAddr, W25QXX_SPI_PAGESIZE);
        WriteAddr +=  W25QXX_SPI_PAGESIZE;
        pBuffer += W25QXX_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        W25Qxx_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

//Reading data from flash
void W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  W25Qxx_SPI_CS_Set(0);

  W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);

  W25Qxx_SPI_Read_Write_Byte((ReadAddr & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((ReadAddr& 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(ReadAddr & 0xFF);

  while (NumByteToRead--)
  {
    *pBuffer = W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
    pBuffer++;
  }

  W25Qxx_SPI_CS_Set(1);
}

//Read ID
uint32_t W25Qxx_ReadID(void)
{
  uint32_t Temp = 0;

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_ID);

  Temp = W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
  Temp = (Temp<<8) | W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
  Temp = (Temp<<8) | W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);

  W25Qxx_SPI_CS_Set(1);
  return Temp;
}

//Sector erase
void W25Qxx_EraseSector(uint32_t SectorAddr)
{
  W25Qxx_WriteEnable();

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_SECTOR_ERASE);
  W25Qxx_SPI_Read_Write_Byte((SectorAddr & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((SectorAddr & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(SectorAddr & 0xFF);
  W25Qxx_SPI_CS_Set(1);

  W25Qxx_WaitForWriteEnd();
}

//Block erase
void W25Qxx_EraseBlock(uint32_t BlockAddr)
{
  W25Qxx_WriteEnable();

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_BLOCK_ERASE);
  W25Qxx_SPI_Read_Write_Byte((BlockAddr & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((BlockAddr & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(BlockAddr & 0xFF);
  W25Qxx_SPI_CS_Set(1);

  W25Qxx_WaitForWriteEnd();
}

//Full-chip erase
void W25Qxx_EraseBulk(void)
{
  W25Qxx_WriteEnable();

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_FLASH__BE);
  W25Qxx_SPI_CS_Set(1);

  W25Qxx_WaitForWriteEnd();
}

//Read flash Size/Capacity
uint32_t W25Qxx_ReadCapacity(void)
{
  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_ID);

  uint8_t cap;
  W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
  W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
  cap = W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);

  W25Qxx_SPI_CS_Set(1);
  for (uint8_t i = 0; i < sizeof(cap_ID); i++)
  {
    if (cap == cap_ID[i])
    {
      return flash_size[i];
    }
  }
  return 0;
}
