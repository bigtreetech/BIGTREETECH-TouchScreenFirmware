#ifndef _TFT_800_480_H_
#define _TFT_800_480_H_

#define LCD_HARDWARE_WIDTH  800
#define LCD_HARDWARE_HEIGHT 480

#define LCD_WIDTH  800
#define LCD_HEIGHT 480

#ifndef BYTE_HEIGHT
  #define BYTE_HEIGHT 24
#endif
#ifndef BYTE_WIDTH
  #define BYTE_WIDTH  (BYTE_HEIGHT / 2)
#endif

#ifndef LARGE_BYTE_HEIGHT
  #define LARGE_BYTE_HEIGHT 32
#endif
#ifndef LARGE_BYTE_WIDTH
  #define LARGE_BYTE_WIDTH  18
#endif

#define ICON_WIDTH   160
#define ICON_HEIGHT  140
#define TITLE_END_Y  50                  // title height
#define ICON_START_Y (TITLE_END_Y + 10)  // top margin

#define LIST_ICON_WIDTH  140
#define LIST_ICON_HEIGHT 110

#define INFOBOX_WIDTH    360
#define INFOBOX_HEIGHT   140

#define SMALLICON_WIDTH  24
#define SMALLICON_HEIGHT 24

// Status Screen menu
#define SS_ICON_COUNT            5
#define SS_ICON_WIDTH            130
#define SS_ICON_HEIGHT           160
//#define SS_SPACE_X             ((LCD_WIDTH - SS_ICON_WIDTH * 5) - ((LCD_WIDTH - ICON_WIDTH * 4) / 4) * 3.5)
#define SS_SPACE_X               (((LCD_WIDTH - (SS_ICON_WIDTH * SS_ICON_COUNT + (LCD_WIDTH - ICON_WIDTH * 4) / 4)) / (SS_ICON_COUNT - 1)) + 0.5)
#define SS_ICON_NAME_Y0          110
#define SS_ICON_VAL_Y0           38
#define SS_ICON_VAL_Y0_2         70
#define STATUS_MSG_ICON_XOFFSET  5
#define STATUS_MSG_ICON_YOFFSET  5
#define STATUS_MSG_TITLE_XOFFSET 5
#define STATUS_MSG_BODY_XOFFSET  2
#define STATUS_MSG_BODY_YOFFSET  75
#define STATUS_MSG_BODY_BOTTOM   29
#define STATUS_GANTRY_YOFFSET    6

#define SS_ICON_TITLE_FONT_SIZE FONT_SIZE_NORMAL
#define SS_ICON_VAL_FONT_SIZE   FONT_SIZE_LARGE
#define SS_ICON_VAL_FONT_SIZE_2 FONT_SIZE_NORMAL

// Preheat menu
#define PREHEAT_TITLE_Y 6
#define PREHEAT_TOOL_Y  65
#define PREHEAT_BED_Y   105

// Printing menu
#define PS_ICON_LG_WIDTH  255
#define PS_ICON_SM_WIDTH  201
#define PS_ICON_HEIGHT    75
#define PS_ICON_SPACE_X   ((LCD_WIDTH - PS_ICON_LG_WIDTH * 2 - PS_ICON_SM_WIDTH - SPACE_X) / 2)
#define PS_ICON_SPACE_Y   ((ICON_HEIGHT + SPACE_Y - PS_ICON_HEIGHT * 2) / 4)
#define PS_ICON_START_Y   (TITLE_END_Y + 5)
#define PS_ICON_TITLE_X   80
#define PS_ICON_TITLE_Y   1
#define PS_ICON_VAL_X     75
#define PS_ICON_VAL_Y     (PS_ICON_HEIGHT / 2)
#define PS_ICON_VAL_LG_EX 245
#define PS_ICON_VAL_SM_EX 191

// Popup menu
#define POPUP_RECT_WINDOW         {SPACE_X * 2, TITLE_END_Y, LCD_WIDTH - SPACE_X * 2, LCD_HEIGHT - SPACE_Y}
#define POPUP_TITLE_HEIGHT        80
#define POPUP_BOTTOM_HEIGHT       105
#define POPUP_BUTTON_HEIGHT       75
#define POPUP_RECT_SINGLE_CONFIRM {320, LCD_HEIGHT - SPACE_Y - 10 - POPUP_BUTTON_HEIGHT, 480, LCD_HEIGHT - SPACE_Y - 10}
#define POPUP_RECT_DOUBLE_CONFIRM {140, LCD_HEIGHT - SPACE_Y - 10 - POPUP_BUTTON_HEIGHT, 300, LCD_HEIGHT - SPACE_Y - 10}
#define POPUP_RECT_DOUBLE_CANCEL  {500, LCD_HEIGHT - SPACE_Y - 10 - POPUP_BUTTON_HEIGHT, 660, LCD_HEIGHT - SPACE_Y - 10}

// W25Qxx SPI Flash
/*
 * Because of W25Qxx takes 4K as a sector, the entire sector must be erased before writing.
 * The starting address of each file must be 4K(0x1000) aligned in order to avoid erasing the data of the previous file
 */
#define W25QXX_SECTOR_SIZE (0x1000)  // 4096 byte = 4K

#ifndef LOGO_MAX_SIZE
  #define LOGO_MAX_SIZE           0xBC000  // 800*480*2 = 0xBB800(+0xBC000) bytes
  #define WORD_UNICODE_SIZE      0x480000  // 24*24/8 * 65536(unicode) = 0x480000 bytes(4.5M)
  #define BYTE_ASCII_SIZE          0x1000  // 24*12/8 * 95(visible ascii) = 0x0D5C (+0x1000 4K)
  #define LARGE_FONT_SIZE          0x3000
  #define _8X16_FONT_SIZE          0x1000
  #define FLASH_SIGN_SIZE          0x1000  // store status of last font/icon/config update
  #define LANGUAGE_SIZE           0x15000
  #define STRINGS_STORE_MAX_SIZE   0x1000  // label strings max size
  #define PREHEAT_STORE_MAX_SIZE   0x1000  // preheat setting max size
  #define PRINT_GCODES_MAX_SIZE    0x5000  // start/end/cancel gcodes  max size
  #define CUSTOM_GCODE_MAX_SIZE    0x5000  // custom gocdes max size
  #define ICON_MAX_SIZE            0xB000  // 160*140*2 = 0xAF00 (+0xB000) per button icon
  #define INFOBOX_MAX_SIZE        0x19000  // 360*140*2 = 0x189C0 (+0x19000)
  #define SMALL_ICON_MAX_SIZE      0x2000  // 24*24*2 = 0x480 (+0x1000) per small icon
#endif

// The offset of the model preview icon in the gcode file
#define MODEL_PREVIEW_OFFSET 0x15355

#endif
