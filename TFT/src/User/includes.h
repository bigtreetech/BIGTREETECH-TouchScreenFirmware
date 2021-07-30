#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include "variants.h"

// standard libs
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// User/HAL/stm32f10x or // HAL/stm32f2_f4xx
#include "lcd_dma.h"
#include "lcd.h"
#include "Serial.h"     // it uses infoSettings. HAL should be independent by that!
#include "spi_slave.h"  // it uses infoSettings. HAL should be independent by that!
#include "spi.h"
#include "timer_pwm.h"
#include "uart.h"

// User/HAL/USB
#include "usbh_msc_core.h"  // HAL/STM32_USB_HOST_Library/Class/MSC/inc
#include "usbh_core.h"      // HAL/STM32_USB_HOST_Library/Core/inc
#include "usbh_usr.h"       // HAL/STM32_USB_HOST_Library/Usr/inc

// User/HAL
#include "buzzer.h"    // it uses infoSettings. HAL should be independent by that!
#include "Knob_LED.h"
#include "LCD_Init.h"
#include "sd.h"
#include "sw_spi.h"
#include "w25qxx.h"
#include "xpt2046.h"

// User/Fatfs
#include "ff.h"
#include "myfatfs.h"

// User/API/Vfs
#include "Vfs/vfs.h"

// User/API/printf
#include "printf/printf.h"

// User/API/Gcode
#include "Gcode/gcode.h"
#include "Gcode/mygcodefs.h"

// User/API/Language
#include "Language.h"
#include "utf8_decode.h"

// User/API/UI
#include "CharIcon.h"
#include "GUI.h"
#include "HD44780_Emulator.h"  // it uses infoSettings
#include "ListItem.h"          // it uses infoSettings
#include "ListManager.h"
#include "Numpad.h"            // it uses infoSettings
#include "ST7920_Emulator.h"   // it uses infoSettings
#include "TouchProcess.h"
#include "ui_draw.h"

// User/API
#include "BabystepControl.h"
#include "boot.h"
#include "CaseLightControl.h"
#include "config.h"
#include "coordinate.h"
#include "debug.h"
#include "extend.h"
#include "FanControl.h"
#include "flashStore.h"
#include "HomeOffsetControl.h"
#include "interfaceCmd.h"
#include "LCD_Colors.h"
#include "LCD_Dimming.h"
#include "LCD_Encoder.h"
#include "LED_Colors.h"
#include "MachineParameters.h"
#include "menu.h"
#include "Notification.h"
#include "parseACK.h"
#include "Printing.h"
#include "ProbeHeightControl.h"
#include "ProbeOffsetControl.h"
#include "ScreenShot.h"
#include "serialConnection.h"
#include "Settings.h"
#include "SpeedControl.h"
#include "Temperature.h"

// User/Menu
#include "ABL.h"
#include "Babystep.h"
#include "BedLeveling.h"
#include "BedLevelingLayer2.h"
#include "BLTouch.h"
#include "CaseLight.h"
#include "common.h"
#include "ConnectionSettings.h"
#include "Extrude.h"
#include "Fan.h"
#include "FeatureSettings.h"
#include "Heat.h"
#include "Home.h"
#include "LEDColor.h"
#include "LevelCorner.h"
#include "Leveling.h"
#include "LoadUnload.h"
#include "MachineSettings.h"
#include "MainPage.h"
#include "MarlinMode.h"
#include "MBL.h"
#include "MeshEditor.h"
#include "MeshTuner.h"
#include "MeshValid.h"
#include "More.h"
#include "Move.h"
#include "NotificationMenu.h"
#include "ParameterSettings.h"
#include "PersistentInfo.h"
#include "Pid.h"
#include "Popup.h"
#include "PowerFailed.h"
#include "PreheatMenu.h"
#include "Print.h"
#include "PrintingMenu.h"
#include "RRFMacros.h"
#include "ScreenSettings.h"
#include "SelectMode.h"
#include "SettingsMenu.h"
#include "Speed.h"
#include "StatusScreen.h"
#include "Terminal.h"
#include "Touchmi.h"
#include "TuneExtruder.h"
#include "Tuning.h"
#include "UnifiedHeat.h"
#include "UnifiedMove.h"
#include "ZOffset.h"

// User
#include "delay.h"
#include "my_misc.h"
#include "os_timer.h"
#include "SanityCheck.h"

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
