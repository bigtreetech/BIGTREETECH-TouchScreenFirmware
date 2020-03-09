/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "usb_conf.h"
#include "usbh_msc_core.h"
#include "usbh_usr.h"
#include "variants.h"

#ifdef SD_SPI_SUPPORT
  #include "sd.h"
#else
  #include "sdio_sdcard.h"
#endif

/* Definitions of physical drive number for each drive */\
#define DEV_MMC		0	/* MMC/SD card to physical drive 0 */
#define DEV_USB		1	/* USB disk to physical drive 1 */

static volatile DSTATUS diskStatus[FF_VOLUMES] = {STA_NOINIT, STA_NOINIT};	/* Disk status */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
  if (pdrv >= FF_VOLUMES)	return STA_NOINIT;

	return diskStatus[pdrv];
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
  for(u8 i = 0; i < FF_VOLUMES; i++)
  {
    diskStatus[i] = STA_NOINIT;
  }

	switch (pdrv) {
	case DEV_MMC :
		if (SD_Init() == 0)
    {
      diskStatus[pdrv] &= ~STA_NOINIT;
    }
    break;

	case DEV_USB :
		if (HCD_IsDeviceConnected(&USB_OTG_Core))
    {
      diskStatus[pdrv] &= ~STA_NOINIT;
    }
    break;

    default:
      return RES_PARERR;
	}
	return diskStatus[pdrv];
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
  if (!count) return RES_PARERR;
  if (diskStatus[pdrv] & STA_NOINIT) return RES_NOTRDY;

	switch (pdrv) {
  case DEV_MMC :
		while (SD_ReadDisk(buff,sector,count)) // read error
    {
      SD_Init();	//init again
    }
    return RES_OK;

	case DEV_USB :
    if(USBH_UDISK_Read(buff, sector, count) == 0)
      return RES_OK;
    else
      return RES_ERROR;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
  if (!count) return RES_PARERR;
  if (diskStatus[pdrv] & STA_NOINIT) return RES_NOTRDY;
  if (diskStatus[pdrv] & STA_PROTECT) return RES_WRPRT;

	switch (pdrv) {
	case DEV_MMC :
		while (SD_WriteDisk((u8*)buff,sector,count)) // write error
    {
      SD_Init();	// init again
    }
    return RES_OK;

	case DEV_USB :
		if(USBH_UDISK_Write((u8*)buff, sector, count) == 0)
      return RES_OK;
    else
      return RES_ERROR;
  }

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch (pdrv) {
	case DEV_MMC :
    return RES_OK;

	case DEV_USB :
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
	}

	return RES_PARERR;
}
