#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include "variants.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "my_misc.h"
#include "printf/printf.h"

#include "os_timer.h"
#include "delay.h"

#include "boot.h"
#include "ScreenShot.h"

#include "Colors.h"
#include "lcd.h"
#include "LCD_Init.h"
#include "lcd_dma.h"
#include "GUI.h"
#include "Language.h"
#include "utf8_decode.h"

#include "uart.h"
#include "Serial.h"
#include "spi.h"
#include "sw_spi.h"
#include "CircularQueue.h"
#include "spi_slave.h"
#include "timer_pwm.h"

#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"

#include "sd.h"
#include "w25qxx.h"
#include "xpt2046.h"
#include "buzzer.h"

#include "LCD_Encoder.h"
#include "ST7920_Emulator.h"
#include "HD44780_Emulator.h"
#include "ui_draw.h"
#include "touch_process.h"
#include "serialConnection.h"
#include "interfaceCmd.h"
#include "coordinate.h"
#include "ff.h"
#include "Vfs/vfs.h"
#include "myfatfs.h"
#include "Gcode/gcode.h"
#include "Gcode/mygcodefs.h"
#include "flashStore.h"
#include "parseACK.h"
#include "Selectmode.h"
#include "MarlinMode.h"
#include "Temperature.h"
#include "Settings.h"
#include "Printing.h"
#include "MachineParameters.h"
#include "FanControl.h"
#include "SpeedControl.h"
#include "BabystepControl.h"
#include "ProbeOffsetControl.h"
#include "ProbeHeightControl.h"
#include "HomeOffsetControl.h"
#include "CaseLightControl.h"

#include "extend.h"
#include "menu.h"
#include "list_item.h"
#include "list_widget.h"
#include "common.h"
#include "Popup.h"
#include "Numpad.h"
#include "Notification.h"
#include "SanityCheck.h"

//menu
#include "MainPage.h"
#include "Heat.h"
#include "PreheatMenu.h"
#include "Move.h"
#include "Home.h"
#include "Print.h"
#include "Printing.h"
#include "More.h"
#include "Speed.h"
#include "ledcolor.h"
#include "Parametersetting.h"
#include "NotificationMenu.h"

#include "Babystep.h"
#include "Extrude.h"
#include "LoadUnload.h"
#include "RRFMacros.h"
#include "Fan.h"
#include "SettingsMenu.h"
#include "PrintingMenu.h"
#include "ScreenSettings.h"
#include "MachineSettings.h"
#include "FeatureSettings.h"
#include "SendGcode.h"
#include "Leveling.h"
#include "BedLeveling.h"
#include "BedLevelingLayer2.h"
#include "MBL.h"
#include "ABL.h"
#include "BLTouch.h"
#include "Touchmi.h"
#include "ZOffset.h"
#include "PowerFailed.h"

#include "UnifiedMove.h"
#include "UnifiedHeat.h"
#include "StatusScreen.h"

#include "Tuning.h"
#include "Pid.h"
#include "TuneExtruder.h"
#include "ConnectionSettings.h"
#include "MeshTuner.h"
#include "MeshEditor.h"
#include "CaseLight.h"
#include "MeshValid.h"

#define MAX_MENU_DEPTH 10       // max sub menu depth
typedef void (*FP_MENU)(void);

typedef struct
{
  FP_MENU menu[MAX_MENU_DEPTH];  // Menu function buffer
  uint8_t cur;                   // Current menu index in buffer
} MENU;

extern MENU infoMenu;

typedef struct
{
  bool wait;              //Whether wait for Marlin's response
  bool rx_ok[_UART_CNT];  //Whether receive Marlin's response or get Gcode by other UART(ESP3D/OctoPrint)
  bool connected;         //Whether have connected to Marlin
  bool printing;          //Whether the host is busy in printing execution. (USB serial printing and GCODE print from onboard)
} HOST;

extern HOST infoHost;

typedef struct
{
  RCC_ClocksTypeDef rccClocks;
  uint32_t PCLK1_Timer_Frequency;
  uint32_t PCLK2_Timer_Frequency;
} CLOCKS;

extern CLOCKS mcuClocks;

#endif