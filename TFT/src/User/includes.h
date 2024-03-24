#ifndef _INCLUDES_H_
#define _INCLUDES_H_

// global includes (always first)
#include "variants.h"
#include "main.h"

// standard libs
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// User
#include "delay.h"
#include "my_misc.h"
#include "os_timer.h"
#include "SanityCheck.h"

// User/HAL/stm32f10x, HAL/stm32f2_f4xx, HAL/gd32f20x or HAL/gd32f30x
#include "lcd_dma.h"
#include "lcd.h"
#include "Serial.h"
#include "spi_slave.h"  // it uses infoSettings. HAL should be independent by that!
#include "spi.h"
#include "timer_pwm.h"
#include "uart.h"

// User/HAL/USB
#include "usbh_msc_core.h"  // HAL/STM32_USB_HOST_Library/Class/MSC/inc
#include "usbh_core.h"      // HAL/STM32_USB_HOST_Library/Core/inc
#include "usbh_usr.h"       // HAL/STM32_USB_HOST_Library/Usr/inc

// User/HAL
#include "buzzer.h"
#include "Knob_LED.h"
#include "LCD_Encoder.h"
#include "LCD_Init.h"
#include "sd.h"
#include "sw_spi.h"
#include "w25qxx.h"
#include "xpt2046.h"

// User/Fatfs
#include "ff.h"
#include "myfatfs.h"

// User/API
#include "Gcode/gcode.h"
#include "Gcode/mygcodefs.h"
#include "printf/printf.h"
#include "Vfs/vfs.h"

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
#include "ui_draw.h"

// User/API
#include "AddonHardware.h"
#include "BabystepControl.h"
#include "boot.h"
#include "BuzzerControl.h"
#include "comment.h"
#include "config.h"
#include "coordinate.h"
#include "debug.h"
#include "FanControl.h"
#include "FlashStore.h"
#include "HomeOffsetControl.h"
#include "HW_Init.h"
#include "LCD_Colors.h"
#include "LCD_Dimming.h"
#include "LED_Colors.h"
#include "LED_Event.h"
#include "LevelingControl.h"
#include "MachineParameters.h"
#include "Mainboard_AckHandler.h"
#include "Mainboard_CmdControl.h"
#include "Mainboard_CmdHandler.h"
#include "Mainboard_FlowControl.h"
#include "menu.h"
#include "ModeSwitching.h"
#include "Notification.h"
#include "PowerFailed.h"
#include "Printing.h"
#include "ProbeHeightControl.h"
#include "ProbeOffsetControl.h"
#include "ScreenShot.h"
#include "SerialConnection.h"
#include "Settings.h"
#include "SpeedControl.h"
#include "Temperature.h"
#include "Touch_Encoder.h"
#include "Touch_Screen.h"

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
#include "Monitoring.h"
#include "More.h"
#include "Move.h"
#include "NotificationMenu.h"
#include "ParameterSettings.h"
#include "PersistentInfo.h"
#include "MPC.h"
#include "Pid.h"
#include "Popup.h"
#include "PreheatMenu.h"
#include "Print.h"
#include "PrintingMenu.h"
#include "PrintRestore.h"
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

#endif
