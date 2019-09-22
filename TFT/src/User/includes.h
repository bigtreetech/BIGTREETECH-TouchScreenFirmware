#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include "variants.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "my_misc.h"

#include "variants.h"
#include "os_timer.h"
#include "delay.h"

#include "boot.h"

#include "lcd.h"
#include "LCD_Init.h"
#include "lcd_dma.h"
#include "GUI.h"
#include "Language.h"

#include "usart.h"
#include "Serial.h"
#include "spi.h"
#include "sw_spi.h"
#include "spi_slave.h"

#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"

#include "sd.h"
#include "w25qxx.h"
#include "xpt2046.h"

#include "LCD_Encoder.h"
#include "ST7920_Simulator.h"
#include "ui_draw.h"
#include "touch_process.h"
#include "interfaceCmd.h"
#include "coordinate.h"
#include "ff.h"
#include "Vfs/vfs.h"
#include "myfatfs.h"
#include "Gcode/gcode.h"
#include "Gcode/mygcodefs.h"
#include "flashStore.h"
#include "parseACK.h"

#include "extend.h"

//menu
#include "menu.h"
#include "MainPage.h"

#include "Heat.h"
#include "Move.h"
#include "Home.h"
#include "Print.h"
#include "Printing.h"
#include "Speed.h"
#include "BabyStep.h"

#include "Extrude.h"
#include "Fan.h"
#include "Settings.h"
#include "leveling.h"
#include "PowerFailed.h"

#include "Popup.h"
#include "Mode.h"

//address in spiflash W25Qxx
#define LOGO_ADDR               0x0
#define ICON_ADDR(num)          ((num)*0x5000+0x4B000)

#define BYTE_ADDR               0x200000
#define WORD_ADDR               0x300000

#define UNI2OEM_ADDR            0x600000
#define OEM2UNI_ADDR            0x700000


#define MAX_MENU_DEPTH 10       // max sub menu depth
typedef void (*FP_MENU)(void); 

typedef struct
{
  FP_MENU menu[MAX_MENU_DEPTH];  // Menu function buffer
  u8      cur;                   // Current menu index in buffer
}MENU;

extern MENU infoMenu;
enum
{
  ICON_BACKGROUND = -1,
  ICON_HEAT = 0,
  ICON_MOVE,
  ICON_HOME,
  ICON_PRINT,
  ICON_EXTRUDE,
  ICON_FAN,
  ICON_SETTINGS,
  ICON_LEVELING,
  
  ICON_INC,
  ICON_DEC,
  ICON_NOZZLE,
  ICON_BED,
  ICON_1_DEGREE,
  ICON_5_DEGREE,
  ICON_10_DEGREE,
  ICON_STOP,
  ICON_BACK,
  ICON_X_INC,
  ICON_Y_INC,
  ICON_Z_INC,
  ICON_01_MM,
  ICON_1_MM,
  ICON_10_MM,
  ICON_X_DEC,
  ICON_Y_DEC,
  ICON_Z_DEC,
  
  ICON_X_HOME,
  ICON_Y_HOME,
  ICON_Z_HOME,
  
  ICON_FOLDER,
  ICON_FILE,
  ICON_PAGE_UP,
  ICON_PAGE_DOWN,
  ICON_PAUSE,
  ICON_RESUME,
  
  ICON_LOAD,
  ICON_UNLOAD,
  ICON_SLOW_SPEED,
  ICON_NORMAL_SPEED,
  ICON_FAST_SPEED,
  ICON_E_1_MM,
  ICON_E_5_MM,
  ICON_E_10_MM,
  
  ICON_FAN_FULL_SPEED,
  ICON_FAN_HALF_SPEED,
  ICON_ROTATE_UI,
  ICON_LANGUAGE,
  ICON_TOUCHSCREEN_ADJUST,
  ICON_SCREEN_INFO,
  ICON_DISCONNECT,
  ICON_BAUDRATE,
  
  ICON_PERCENTAGE,
  ICON_BABYSTEP,
  ICON_001_MM,
  ICON_BSD_SOURCE,
  ICON_SD_SOURCE,
  ICON_U_DISK,
  ICON_RUNOUT,
  
  ICON_POINT_1,
  ICON_POINT_2,
  ICON_POINT_3,
  ICON_POINT_4,
  ICON_ABL,
//  ICON_RESERVE
};



typedef struct
{	
  bool wait;       //Whether wait for Marlin's response
  bool rx_ok[_USART_CNT]; //Whether receive Marlin's response or get Gcode by other UART(ESP3D/OctoPrint)
  bool connected;  //Whether have connected to Marlin
#ifdef ONBOARD_SD_SUPPORT     
  bool printing;   //Whether the host is busy in printing execution. ( USB serial printing and GCODE print from onboard)
#endif  
}HOST;

extern HOST infoHost;

#endif

