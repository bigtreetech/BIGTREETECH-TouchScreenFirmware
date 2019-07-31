#ifndef _BOOT_H_
#define _BOOT_H_

#include "variants.h"

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
