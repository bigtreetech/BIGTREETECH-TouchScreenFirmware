#ifndef _BOOT_H_
#define _BOOT_H_

#include "variants.h"

#define W25QXX_SECTOR_SIZE (0x1000) // 4096-4K

//address in spiflash W25Qxx
#define LOGO_ADDR               0x0
#define ICON_ADDR(num)          ((num)*0x5000+0x4B000)

#define UNI2OEM_ADDR            0x200000 // Fatfs unicode to ansi/oem convert
#define OEM2UNI_ADDR            0x216000
//  0x39D
#define BYTE_ASCII_ADDR         0x22C000 // ascii
#define WORD_CJK_ADDR           0x22D000 // cjk
#define BYTE_CYRILLIC_ADDR      0x39D000 // russia
#define WORD_JAPANESE_ADDR      0x39E000 // japanese
#define BYTE_ARMENIAN_ADDR      0x3A2000 // armenian
#define BYTE_LATIN_ADDR         0x3A3000 // latin 1 Supplement,extended-A&B
#define WORD_KOREAN_ADDR        0x3A8000 // Korean(Hangul Syllables)


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
