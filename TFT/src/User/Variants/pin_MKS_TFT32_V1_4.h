#ifndef _PIN_MKS_TFT32_V1_4_H_ // modify to actual filename !!!
#define _PIN_MKS_TFT32_V1_4_H_ // modify to actual filename !!!

// MCU type (STM32F10x, STM32F2xx)
#include "stm32f10x.h"

// LCD resolution, font and icon size
#ifndef TFT_RESOLUTION
  #define TFT_RESOLUTION
  #include "./Resolution/TFT_320X240.h"
#endif

#ifndef ROOT_DIR
  #define ROOT_DIR "MKS"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT32_V1.4"
#endif

// LCD interface
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER HX8558  // Type of LCD driver, now support[RM68042, ILI9488, ILI9341, ILI9325, ST7789, HX8558].
#endif
//#define STM32_HAS_FSMC  // FSMC 8080 interface(high speed), or normal IO interface(low speed)
#ifndef LCD_DATA_16BIT
  #define LCD_DATA_16BIT 1  // LCD data 16bit or 8bit
#endif

// Debug disable, free pins for other function
//#define DISABLE_JTAG   // free JTAG(PB3/PB4) for SPI3
//#define DISABLE_DEBUG  // free all pins

// LCD Backlight pin (PWM can adjust brightness)
//#define LCD_LED_PIN           PD14
//#define LCD_LED_PIN_ALTERNATE 0
//#define LCD_LED_PWM_CHANNEL   _TIM4_CH3

// SERIAL_PORT: communicating with host(Marlin, smoothieware, etc...)
// SERIAL_PORT_X: communicating with other controller(Octoprint, ESP3D, other UART Touch Screen, etc...)
#define SERIAL_PORT   _USART2  // default usart port
#define SERIAL_PORT_2 _USART1
#define SERIAL_PORT_3 _USART3
#define USART2_TX_PIN PD5
#define USART2_RX_PIN PD6
#define USART3_TX_PIN PD8
#define USART3_RX_PIN PD9
//#define SERIAL_PORT_4 _UART4

// Serial port for debugging
#ifdef SERIAL_DEBUG_ENABLED
  #define SERIAL_DEBUG_PORT SERIAL_PORT_3
#endif

// XPT2046 Software SPI Pins (touch screen ic)
// need CS/SCK/MISO/MOSI for Software SPI, and TPEN for pen interrupt
#define XPT2046_CS   PC9
#define XPT2046_SCK  PC10
#define XPT2046_MISO PC11
#define XPT2046_MOSI PC12
#define XPT2046_TPEN PC5

// SD Card SPI pins
#define SD_SPI_SUPPORT
//#define SD_SDIO_SUPPORT
#ifdef SD_SPI_SUPPORT
  #define SD_LOW_SPEED  7      // 2^(SPEED+1) = 256 frequency division
  #define SD_HIGH_SPEED 1      // 2 frequency division
  #define SD_SPI        _SPI1
  #define SD_CS_PIN     PD11
#endif

// SD Card CD detect pin
#define SD_CD_PIN PB15

// W25Qxx SPI pins
#define W25Qxx_SPEED  1
#define W25Qxx_SPI    _SPI1
#define W25Qxx_CS_PIN PB9

//
//----------------------------------------------------------------------------
// How to setup Marlin mode (LCD12864 Emulator) on MKS Gen L V1.0 or SKR V1.3
//----------------------------------------------------------------------------
//
// In order to use Marlin mode (12864 emulation mode), you need to make changes on both the HW and FW of the MKS TFT board
// and the FW of the Marlin board.
//
// The following sections provide the full setup procedure to allow Marlin mode for the Marlin boards reported below:
//  1) MKS Gen L V1.0
//  2) SKR V1.3
//
//
// ------------------------------------
// MKS TFT HW changes:
// ------------------------------------
//
// On the MKS TFT, the SPI3 is used for Marlin mode. PB3 pin on the SPI3 (pin number 89 on the TFT's STM32 chip) is also needed
// but that pin is not exposed to any pin header on the TFT board. This means that PB3 pin must be wired from the TFT's STM32 chip
// to a free pin header (e.g. the NC pin on the WiFi connector) on the TFT board.
//
// NOTES:
//  1) Wiring PB3 pin to a free pin header on the TFT board requires good soldering skill.
//     If you don't have good soldering skill, DO NOT try to make any change. Otherwise, you will probably damage the TFT board.
//  2) By default, SPI3_PIN_SMART_USAGE is enabled. When enabled, pin PB0 and PB1 are not needed for Marlin mode.
//     These pins are normally used for the Power Off and Filament Runout features, so they will continue be available in Marlin mode.
//     If SPI3_PIN_SMART_USAGE is disabled, the Power Off and Filament Runout features will be no more available in Marlin mode.
//  3) To reduce the effect of EMI, it is strongly suggested to use single cables (possibly shielded) for all the SPI pins
//     (SPI3_SCK, SPI3_MOSI_PIN and SPI3_CS_PIN). For the encoder pins, a flat cable can be used.
//  4) In case LCD Encoder's sliding buttons (pin LCD_ENCA_PIN and LCD_ENCB_PIN) don't produce any movement on menu,
//     try to increase the delay LCD_ENCODER_DELAY in Configuration.h (e.g. 64).
//
//
// ------------------------------------
// FW and HW connection schema:
// ------------------------------------
//
// MKS TFT FW pins   MKS TFT HW pins / board pins         MKS Gen L V1.0 HW pins   SKR V1.3 HW pins   Marlin FW pins
// ---------------   -----------------------------        ----------------------   ----------------   ---------------
// LCD_ENCA_PIN      PA13 / JTAG DIO                 =>   EXP2 D31                 EXPA2_08_PIN       BTN_EN1
// LCD_ENCB_PIN      PA14 / JTAG CLK                 =>   EXP2 D33                 EXPA2_06_PIN       BTN_EN2
// LCD_BTN_PIN       PB0  / PB0                      =>   EXP1 D35                 EXPA2_07_PIN       BTN_ENC
// SPI3_CS_PIN       PB1  / PB1                      =>   EXP1 D16                 EXPA2_04_PIN       LCD_PINS_RS
// SPI3_SCK_PIN      PB3  / NC WiFi (wired to PB3)   =>   EXP1 D23                 EXPA2_09_PIN       LCD_PINS_D4
// SPI3_MOSI_PIN     PB5  / PB5                      =>   EXP1 D17                 EXPA2_05_PIN       LCD_PINS_ENABLE
//
//
// ------------------------------------
// MKS Gen L V1.0 pinout schema:
// ------------------------------------
//                       ______                                              ______
//                  D50 | 1  2 | D52                      (BEEPER_PIN)  D37 | 1  2 | D35  (BTN_ENC)
//       (BTN_EN1)  D31 | 3  4 | D53  (SDSS)         (LCD_PINS_ENABLE)  D17 | 3  4 | D16  (LCD_PINS_RS)
//       (BTN_EN2)  D33 | 5  6   D51                     (LCD_PINS_D4)  D23 | 5  6   D25  (LCD_PINS_D5)
// (SD_DETECT_PIN)  D49 | 7  8 | RST                     (LCD_PINS_D6)  D27 | 7  8 | D29  (LCD_PINS_D7)
//                  GND | 9 10 | D41  (KILL_PIN)                        GND | 9 10 | 5V
//                       ------                                              ------
//                        EXP2                                                EXP1
//
//
// ------------------------------------
// MKS Gen L V1.0 FW (Marlin) changes:
// ------------------------------------
//
// In the Configuration.h file uncomment and set the following existing parameters as reported below:
//
// #define REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER
//
// #define ENCODER_PULSES_PER_STEP 4
//
//
// ------------------------------------
// SKR V1.3 FW (Marlin) changes:
// ------------------------------------
//
// In the Configuration.h file uncomment and set the following existing parameters as reported below:
//
// #define CR10_STOCKDISPLAY
//
// #define ENCODER_PULSES_PER_STEP 4
//
//
// In the pins_BTT_SKR_V1_3.h file define pins as reported below:
//
// #elif ENABLED(CR10_STOCKDISPLAY)
//
//   #define LCD_PINS_RS             EXPA2_04_PIN  // CS
//   #define BTN_EN1                 EXPA2_08_PIN
//   #define BTN_EN2                 EXPA2_06_PIN
//   #define BTN_ENC                 EXPA2_07_PIN  // (58) open-drain  EXPA1_09_PIN
//   #define LCD_PINS_ENABLE         EXPA2_05_PIN  // EXPA1_03_PIN  //MOSI
//   #define LCD_PINS_D4             EXPA2_09_PIN  // EXPA1_05_PIN  //CLK
//

// ST7920 Emulator SPI pins
//#define ST7920_EMULATOR  // uncomment to enable Marlin mode
#ifdef ST7920_EMULATOR
  #define ST7920_SPI _SPI3
#endif

#if defined(ST7920_EMULATOR) || defined(LCD2004_EMULATOR)
  #define HAS_EMULATOR
#endif

// Buzzer support
#define BUZZER_PIN PA2

// Marlin mode + LCD Encoder support
#ifdef ST7920_EMULATOR
  #define DISABLE_DEBUG         // free JTAG(PB3/PB4) for SPI3 and free SWDIO PA13 PA14 for encoder pins
  #define LCD_ENCA_PIN  PA13    // map ENCA pin to JTAG DIO pin
  #define LCD_ENCB_PIN  PA14    // map ENCB pin to JTAG CLK pin

  #define SPI3_PIN_SMART_USAGE  // if enabled, it avoids any SPI3 CS pin usage and free the MISO (PB4 pin) for encoder pins
  #ifdef SPI3_PIN_SMART_USAGE
    #define LCD_BTN_PIN PB4     // map BTN pin to PB4 pin
  #else
    #define LCD_BTN_PIN PB0     // map BTN pin to PB0 pin

    #define SPI3_CS_PIN PB1     // CS pin used for SPI3 slave mode mapped to PB1 pin
  #endif
#endif

// U disk support
#define U_DISK_SUPPORT
#define USE_USB_OTG_FS

// Extend function(PS_ON, filament_detect)
#if !defined(ST7920_EMULATOR) || defined(SPI3_PIN_SMART_USAGE)
  #ifndef PS_ON_PIN
    #define PS_ON_PIN PB0
  #endif

  #ifndef FIL_RUNOUT_PIN
    #define FIL_RUNOUT_PIN PB1
  #endif
#endif

//#define LED_COLOR_PIN PC7

#endif
