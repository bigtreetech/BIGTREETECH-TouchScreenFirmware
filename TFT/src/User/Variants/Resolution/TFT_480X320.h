#ifndef _TFT_480_320_H_
#define _TFT_480_320_H_

#define LCD_WIDTH   480
#define LCD_HEIGHT  320

#ifndef BYTE_HEIGHT
  #define BYTE_HEIGHT 24
#endif
#ifndef BYTE_WIDTH
  #define BYTE_WIDTH  (BYTE_HEIGHT/2)
#endif

#define ICON_WIDTH   95
#define ICON_HEIGHT  95
#define TITLE_END_Y  40
#define ICON_START_Y (TITLE_END_Y+10)

#define LIST_ICON_WIDTH   85
#define LIST_ICON_HEIGHT  70

#define INFOBOX_WIDTH     215
#define INFOBOX_HEIGHT    95

#define SMALLICON_WIDTH   24
#define SMALLICON_HEIGHT  24

// Status screen menu
#define SSICON_VAL_Y0             63
#define SSICON_NAME_Y0            7
#define STATUS_MSG_ICON_XOFFSET   5
#define STATUS_MSG_ICON_YOFFSET   5
#define STATUS_MSG_TITLE_XOFFSET  5
#define STATUS_MSG_BODY_XOFFSET   0
#define STATUS_MSG_BODY_YOFFSET   59
#define STATUS_MSG_BODY_BOTTOM    6
#define STATUS_TITLE_SHORT        20
#define STATUS_GANTRY_YOFFSET     6

// Printing menu
#define PICON_LG_WIDTH     154
#define PICON_SM_WIDTH     121
#define PICON_HEIGHT       50
#define PICON_SPACE_X      ((LCD_WIDTH - PICON_LG_WIDTH*2 - PICON_SM_WIDTH - SPACE_X )/2)
#define PICON_SPACE_Y      (ICON_HEIGHT + SPACE_Y - PICON_HEIGHT*2)/2
#define PICON_TITLE_X      52
#define PICON_TITLE_Y      1
#define PICON_VAL_X        PICON_TITLE_X
#define PICON_VAL_Y        (PICON_HEIGHT/2)
#define PICON_VAL_LG_EX    150
#define PICON_VAL_SM_EX    117

// Popup menu
#define POPUP_RECT_WINDOW         {80, 50, 400, 270}
#define POPUP_RECT_SINGLE_CONFIRM {180, 210, 300, 260}
#define POPUP_RECT_DOUBLE_CONFIRM {90,  210, 210, 260}
#define POPUP_RECT_DOUBLE_CANCEL  {270, 210, 390, 260}
#define POPUP_TITLE_HEIGHT  40
#define POPUP_TEXT_HEIGHT   110
#define POPUP_BOTTOM_HEIGHT 70

#endif
