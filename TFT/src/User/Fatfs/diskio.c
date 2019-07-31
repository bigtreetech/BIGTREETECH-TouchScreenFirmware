#include "diskio.h"
#include "usb_conf.h"
#include "usbh_msc_core.h"
#include "usbh_usr.h"
#include "variants.h"

#ifdef SD_SPI_SUPPORT
  #include "sd.h"
#else
  #include "sdio_sdcard.h"
#endif

static volatile DSTATUS diskStatus[_VOLUMES] = {STA_NOINIT, STA_NOINIT};	/* Disk status */

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{ 
	return diskStatus[pdrv];
}  

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
  for(u8 i = 0; i < _VOLUMES; i++)
  {
    diskStatus[i] = STA_NOINIT;
  }
  
	switch(pdrv)
	{
    case VOLUMES_SD_CARD://SD卡
      if (SD_Init() == 0)
      {  
        diskStatus[pdrv] &= ~STA_NOINIT;
      }      
      break;
    
    case VOLUMES_U_DISK:
      if (HCD_IsDeviceConnected(&USB_OTG_Core))
      {  
        diskStatus[pdrv] &= ~STA_NOINIT;
      }
      break;
      
    default:
      return RES_PARERR;
	}		 
	return  diskStatus[pdrv];
} 

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
  if (!count) return RES_PARERR;
  if (diskStatus[pdrv] & STA_NOINIT) return RES_NOTRDY;
  
	switch (pdrv)
	{
		case VOLUMES_SD_CARD://SD卡
			while (SD_ReadDisk(buff,sector,count))//读出错
			{
				SD_Init();	//重新初始化SD卡
			}
      return RES_OK;
      
    case VOLUMES_U_DISK:
      if(USBH_UDISK_Read(buff, sector, count) == 0)
        return RES_OK;
      else
        return RES_ERROR;
      
    default:
      return RES_PARERR;
	}   
}

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
  if (!count) return RES_PARERR;
  if (diskStatus[pdrv] & STA_NOINIT) return RES_NOTRDY;
  if (diskStatus[pdrv] & STA_PROTECT) return RES_WRPRT; 	 
  
  switch (pdrv)
  {
    case VOLUMES_SD_CARD://SD卡
      while (SD_WriteDisk((u8*)buff,sector,count))//写出错
      {
        SD_Init();	//重新初始化SD卡
      }
      return RES_OK;
    
    case VOLUMES_U_DISK:
      if(USBH_UDISK_Write((u8*)buff, sector, count) == 0)
        return RES_OK;
      else
        return RES_ERROR;
    
    default:
      return RES_PARERR;
  }
}
#endif

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  if (diskStatus[pdrv] & STA_NOINIT) return RES_NOTRDY;
  
  switch (pdrv)
  {
    case VOLUMES_SD_CARD:
      return RES_OK;
    
    case VOLUMES_U_DISK:
    {
      DRESULT res = RES_ERROR;
      switch (cmd) 
      {
        case CTRL_SYNC :		/* Make sure that no pending write process */
          res = RES_OK;
          break;

        case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
          *(DWORD*)buff = (DWORD) USBH_MSC_Param.MSCapacity;
          res = RES_OK;
          break;

        case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
          *(WORD*)buff = 512;
          res = RES_OK;
          break;

        case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
          *(DWORD*)buff = 512;
          break;

        default:
          res = RES_PARERR;
      }
      return res;
    }
      
    default:
      return RES_PARERR;
  }
}
#endif
 

#include "stdlib.h"
void *ff_memalloc(UINT msize)
{
	return malloc(msize);
}
void ff_memfree(void *mblock)
{
	free(mblock);
}
DWORD get_fattime (void)
{
	u32 date;
	date=(	((2019-1980)<<25)|(3<<21)|(15<<16)|(16<<11)|(40<<5)|(0)  );
	return date;
}

