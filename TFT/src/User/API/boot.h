#ifndef _BOOT_H_
#define _BOOT_H_

#include "stm32f10x.h"
#include "variants.h"

#define BMP		(1<<1)
#define FONT	(1<<2)

#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2)|| defined(TFT35_V3_0)
  #define ROOT_DIR "TFT35"
#elif defined(TFT28)
  #define ROOT_DIR "TFT28"
#elif defined(TFT24_V1_1)
  #define ROOT_DIR "TFT24"
#endif

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
