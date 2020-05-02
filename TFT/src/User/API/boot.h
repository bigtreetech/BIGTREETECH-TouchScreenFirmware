#ifndef _BOOT_H_
#define _BOOT_H_

#include "variants.h"
#include "stdbool.h"

#define W25QXX_SECTOR_SIZE (0x1000) // 4096-4K

//address in spiflash W25Qxx
#define LOGO_ADDR               0x0
#define WORD_UNICODE            0x4B000 // unicode (+0x480000 4.5M)
#define BYTE_ASCII_ADDR         (WORD_UNICODE+0x480000) // ascii (+0x1000 4K)
//#define BYTE_RESERVE_ADDR      0x710000
#define ICON_ADDR(num)          ((num)*0x5000+BYTE_ASCII_ADDR+0x1000)
#define INFOBOX_ADDR            (ICON_ADDR(ICON_PREVIEW) + 0x5000) // total byte size 0xA7F8
#define SMALL_ICON_START_ADDR   (INFOBOX_ADDR+0xA7F8)
#define SMALL_ICON_ADDR(num)    ((num)*0x1000+SMALL_ICON_START_ADDR)

#define ADMIN_MODE_FILE "0:admin.txt"
#define FIRMWARE_NAME STRINGIFY(HARDWARE) "." STRINGIFY(SOFTWARE_VERSION)
#define BMP_ROOT_DIR "0:" ROOT_DIR "/bmp"
#define FONT_ROOT_DIR "0:" ROOT_DIR "/font"
#define TFT_RESET_FILE "0:reset.txt"

//This List is Auto-Generated. Please add new icons in icon_list.inc only
enum
{
#define X_ICON(NAME) ICON_##NAME ,
#include "icon_list.inc"
#undef  X_ICON
//add new icons in icon_list.inc only
//  ICON_RESERVE

// Preview should be in the last place before ICON_BACKGROUND to save flash storage space
  ICON_PREVIEW,
// Back ground sign
  ICON_BACKGROUND
};

typedef enum{
BMP_SUCCESS,
BMP_NOTFOUND,
BMP_NOT24BIT,
BMP_INVALIDFILE
}BMPUPDATE_STAT;

typedef union
{
	u16 color;
	struct{
	u16  b:5;
	u16  g:6;
	u16  r:5;
 }RGB;
}GUI_COLOR;

void scanUpdates(void);
void dispIconFail(u8 *lbl);
bool bmpDecode(char *bmp, u32 addr);

#endif
