#ifndef _BOOT_H_
#define _BOOT_H_

#include "variants.h"

#define W25QXX_SECTOR_SIZE (0x1000) // 4096-4K

//address in spiflash W25Qxx
#define LOGO_ADDR               0x0
#define ICON_ADDR(num)          ((num)*0x5000+0x4B000)

//
#define WORD_UNICODE            0x200000 // unicode (0x480000 4.5M)
#define BYTE_ASCII_ADDR         0x680000 // ascii (0x1000 4K)
#define BYTE_CYRILLIC_ADDR      0x681000 // russia (0x1000 4K)
#define BYTE_LATIN_ADDR         0x682000 // latin 1 Supplement,extended-A&B  (0x5000 20K)


#define BMP		(1<<1)
#define FONT	(1<<2)

#define BMP_ROOT_DIR "0:"ROOT_DIR"/bmp"
#define FONT_ROOT_DIR "0:"ROOT_DIR"/font"


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

#endif 
