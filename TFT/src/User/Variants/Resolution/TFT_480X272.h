#ifndef _TFT_480_272_H_
#define _TFT_480_272_H_

#define LCD_WIDTH   480
#define LCD_HEIGHT  272

#ifndef BYTE_HEIGHT
  #define BYTE_HEIGHT 24
#endif
#ifndef BYTE_WIDTH
  #define BYTE_WIDTH  (BYTE_HEIGHT/2)
#endif

#ifndef LARGE_BYTE_HEIGHT
  #define LARGE_BYTE_HEIGHT 32
#endif
#ifndef LARGE_BYTE_WIDTH
  #define LARGE_BYTE_WIDTH  18
#endif

#define ICON_WIDTH   95
#define ICON_HEIGHT  80
#define TITLE_END_Y  30
#define ICON_START_Y (TITLE_END_Y+10)

#define LIST_ICON_WIDTH   85
#define LIST_ICON_HEIGHT  60

#define INFOBOX_WIDTH     215
#define INFOBOX_HEIGHT    80

#define SMALLICON_WIDTH   65
#define SMALLICON_HEIGHT  55

// Status screen menu
#define SSICON_COUNT              4
#define SSICON_WIDTH              ICON_WIDTH
#define SSICON_HEIGHT             ICON_HEIGHT
#define SSICON_VAL_Y0             62
#define SSICON_VAL_COLOR          BLACK
#define SSICON_NAME_Y0            5
#define SSICON_NAME_COLOR         WHITE
#define STATUS_MSG_ICON_XOFFSET   5
#define STATUS_MSG_ICON_YOFFSET   3
#define STATUS_MSG_TITLE_XOFFSET  5
#define STATUS_MSG_BODY_XOFFSET   0
#define STATUS_MSG_BODY_YOFFSET   44
#define STATUS_MSG_BODY_BOTTOM    6
#define STATUS_GANTRY_YOFFSET     4

#define NAME_LARGE_FONT           false
#define VAL_LARGE_FONT            false

//Heating Menu
#define PREHEAT_TITLE_Y   0
#define PREHEAT_TOOL_Y    35
#define PREHEAT_BED_Y     65

// Printing menu
#define PICON_LG_WIDTH     154
#define PICON_SM_WIDTH     121
#define PICON_HEIGHT       48
#define PICON_SPACE_X      ((LCD_WIDTH - PICON_LG_WIDTH*2 - PICON_SM_WIDTH - SPACE_X )/2)
#define PICON_SPACE_Y      (ICON_HEIGHT + SPACE_Y - PICON_HEIGHT*2)/2
#define PICON_TITLE_X      52
#define PICON_TITLE_Y      0
#define PICON_VAL_X        PICON_TITLE_X
#define PICON_VAL_Y        (PICON_HEIGHT/2)
#define PICON_VAL_LG_EX    150
#define PICON_VAL_SM_EX    117

// Popup menu
#define POPUP_RECT_WINDOW         {SPACE_X*2, TITLE_END_Y, LCD_WIDTH - SPACE_X*2, LCD_HEIGHT - SPACE_Y}
#define POPUP_TITLE_HEIGHT        40
#define POPUP_BOTTOM_HEIGHT       55
#define POPUP_BUTTON_HEIGHT       40
#define POPUP_RECT_SINGLE_CONFIRM {180, LCD_HEIGHT- SPACE_Y - 10 - POPUP_BUTTON_HEIGHT, 300, LCD_HEIGHT - SPACE_Y - 5}
#define POPUP_RECT_DOUBLE_CONFIRM {90,  LCD_HEIGHT- SPACE_Y - 10 - POPUP_BUTTON_HEIGHT, 210, LCD_HEIGHT - SPACE_Y - 5}
#define POPUP_RECT_DOUBLE_CANCEL  {270, LCD_HEIGHT- SPACE_Y - 10 - POPUP_BUTTON_HEIGHT, 390, LCD_HEIGHT - SPACE_Y - 5}

// The offset of the model preview icon in the gcode file
#define MODEL_PREVIEW_OFFSET 0x4D6E

#endif
