#include "LCD_Init.h"
#include "includes.h"  // for RM68042..., lcd.h, lcd_dma.h, timer_pwm.h etc.
#include "GPIO_Init.h"

// LCD driver sequential
#if LCD_DRIVER_HAS(RM68042)
  #include "LCD_Driver/RM68042.h"
#endif
#if LCD_DRIVER_HAS(ILI9488)
  #include "LCD_Driver/ILI9488.h"
#endif
#if LCD_DRIVER_HAS(ILI9341)
  #include "LCD_Driver/ILI9341.h"
#endif
#if LCD_DRIVER_HAS(ST7789)
  #include "LCD_Driver/ST7789.h"
#endif
#if LCD_DRIVER_HAS(HX8558)
  #include "LCD_Driver/HX8558.h"
#endif
#if LCD_DRIVER_HAS(SSD1963)
  #include "LCD_Driver/SSD1963.h"
#endif
#if LCD_DRIVER_HAS(ILI9325)
  #include "LCD_Driver/ILI9325.h"
#endif
#if LCD_DRIVER_HAS(NT35310)
  #include "LCD_Driver/NT35310.h"
#endif
#if LCD_DRIVER_HAS(ST7796S)
  #include "LCD_Driver/ST7796S.h"
#endif

static void (* pLCD_SetDirection)(uint8_t rotate);
static void (* pLCD_SetWindow)(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);

#ifdef LCD_LED_PIN

static inline void LCD_LED_On(void)
{
  #ifdef LCD_LED_PWM_CHANNEL
    LCD_SET_BRIGHTNESS(100);
  #else
    GPIO_SetLevel(LCD_LED_PIN, 1);
  #endif
}

static inline void LCD_LED_Off(void)
{
  #ifdef LCD_LED_PWM_CHANNEL
    LCD_SET_BRIGHTNESS(0);
  #else
    GPIO_SetLevel(LCD_LED_PIN, 0);
  #endif
}

static inline void LCD_LED_Init(void)
{
  #ifdef LCD_LED_PWM_CHANNEL
    GPIO_InitSet(LCD_LED_PIN, MGPIO_MODE_AF_PP, LCD_LED_PIN_ALTERNATE);
    TIM_PWM_Init(LCD_LED_PWM_CHANNEL);
  #else
    LCD_LED_Off();
    GPIO_InitSet(LCD_LED_PIN, MGPIO_MODE_OUT_PP, 0);
  #endif
}

#endif  // LCD_LED_PIN

#ifdef SCREEN_SHOT_TO_SD

static uint32_t (* pLCD_ReadPixel_24Bit)(int16_t x, int16_t y);

uint32_t LCD_ReadPixel_24Bit(int16_t x, int16_t y)
{
  return pLCD_ReadPixel_24Bit(x, y);
}

#endif  // SCREEN_SHOT_TO_SD

static inline void LCD_Init_Sequential(void)
{
  #if LCD_DRIVER_IS(RM68042)
    RM68042_Init_Sequential();
    pLCD_SetDirection = RM68042_SetDirection;
    pLCD_SetWindow = RM68042_SetWindow;
    #ifdef SCREEN_SHOT_TO_SD
      pLCD_ReadPixel_24Bit = RM68042_ReadPixel_24Bit;
    #endif
  #elif LCD_DRIVER_IS(ILI9488)
    ILI9488_Init_Sequential();
    pLCD_SetDirection = ILI9488_SetDirection;
    pLCD_SetWindow = ILI9488_SetWindow;
    #ifdef SCREEN_SHOT_TO_SD
      pLCD_ReadPixel_24Bit = ILI9488_ReadPixel_24Bit;
    #endif
  #elif LCD_DRIVER_IS(ILI9341)
    ILI9341_Init_Sequential();
    pLCD_SetDirection = ILI9341_SetDirection;
    pLCD_SetWindow = ILI9341_SetWindow;
    #ifdef SCREEN_SHOT_TO_SD
      pLCD_ReadPixel_24Bit = ILI9341_ReadPixel_24Bit;
    #endif
  #elif LCD_DRIVER_IS(ST7789)
    ST7789_Init_Sequential();
    pLCD_SetDirection = ST7789_SetDirection;
    pLCD_SetWindow = ST7789_SetWindow;
    #ifdef SCREEN_SHOT_TO_SD
      pLCD_ReadPixel_24Bit = ST7789_ReadPixel_24Bit;
    #endif
  #elif LCD_DRIVER_IS(HX8558)
    HX8558_Init_Sequential();
    pLCD_SetDirection = HX8558_SetDirection;
    pLCD_SetWindow = HX8558_SetWindow;
    #ifdef SCREEN_SHOT_TO_SD
      pLCD_ReadPixel_24Bit = HX8558_ReadPixel_24Bit;
    #endif
  #elif LCD_DRIVER_IS(SSD1963)
    SSD1963_Init_Sequential();
    pLCD_SetDirection = SSD1963_SetDirection;
    pLCD_SetWindow = SSD1963_SetWindow;
    #ifdef SCREEN_SHOT_TO_SD
      pLCD_ReadPixel_24Bit = SSD1963_ReadPixel_24Bit;
    #endif
  #elif LCD_DRIVER_IS(ILI9325)
    ILI9325_Init_Sequential();
    pLCD_SetDirection = ILI9325_SetDirection;
    pLCD_SetWindow = ILI9325_SetWindow;
    #ifdef SCREEN_SHOT_TO_SD
      pLCD_ReadPixel_24Bit = ILI9325_ReadPixel_24Bit;
    #endif
  #elif LCD_DRIVER_IS(NT35310)
    NT35310_Init_Sequential();
    pLCD_SetDirection = NT35310_SetDirection;
    pLCD_SetWindow = NT35310_SetWindow;
    #ifdef SCREEN_SHOT_TO_SD
      pLCD_ReadPixel_24Bit = NT35310_ReadPixel_24Bit;
    #endif
  #elif LCD_DRIVER_IS(ST7796S)
    ST7796S_Init_Sequential();
    pLCD_SetDirection = ST7796S_SetDirection;
    pLCD_SetWindow = ST7796S_SetWindow;
    #ifdef SCREEN_SHOT_TO_SD
      pLCD_ReadPixel_24Bit = ST7796S_ReadPixel_24Bit;
    #endif
  #else
      if (0)
      {}
    #if LCD_DRIVER_HAS(RM68042)
      else if (LCD_DriveIsRM68042())
      {
        RM68042_Init_Sequential();
        pLCD_SetDirection = RM68042_SetDirection
        pLCD_SetWindow = RM68042_SetWindow;
        #ifdef SCREEN_SHOT_TO_SD
          pLCD_ReadPixel_24Bit = RM68042_ReadPixel_24Bit;
        #endif
      }
    #endif
    #if LCD_DRIVER_HAS(ILI9488)
      else if (LCD_DriveIsILI9488())
      {
        ILI9488_Init_Sequential();
        pLCD_SetDirection = ILI9488_SetDirection;
        pLCD_SetWindow = ILI9488_SetWindow;
        #ifdef SCREEN_SHOT_TO_SD
          pLCD_ReadPixel_24Bit = ILI9488_ReadPixel_24Bit;
        #endif
      }
    #endif
    #if LCD_DRIVER_HAS(ILI9341)
      else if (LCD_DriveIsILI9341())
      {
        ILI9341_Init_Sequential();
        pLCD_SetDirection = ILI9341_SetDirection;
        pLCD_SetWindow = ILI9341_SetWindow;
        #ifdef SCREEN_SHOT_TO_SD
          pLCD_ReadPixel_24Bit = ILI9341_ReadPixel_24Bit;
        #endif
      }
    #endif
    #if LCD_DRIVER_HAS(ST7789)
      else if (LCD_DriveIsST7789())
      {
        ST7789_Init_Sequential();
        pLCD_SetDirection = ST7789_SetDirection;
        pLCD_SetWindow = ST7789_SetWindow;
        #ifdef SCREEN_SHOT_TO_SD
          pLCD_ReadPixel_24Bit = ST7789_ReadPixel_24Bit;
        #endif
      }
    #endif
    #if LCD_DRIVER_HAS(HX8558)
      else if (LCD_DriveIsHX8558())
      {
        HX8558_Init_Sequential();
        pLCD_SetDirection = HX8558_SetDirection;
        pLCD_SetWindow = HX8558_SetWindow;
        #ifdef SCREEN_SHOT_TO_SD
          pLCD_ReadPixel_24Bit = HX8558_ReadPixel_24Bit;
        #endif
      }
    #endif
    #if LCD_DRIVER_HAS(SSD1963)
      else if (LCD_DriveIsSSD1963())
      {
        SSD1963_Init_Sequential();
        pLCD_SetDirection = SSD1963_SetDirection;
        pLCD_SetWindow = SSD1963_SetWindow;
        #ifdef SCREEN_SHOT_TO_SD
          pLCD_ReadPixel_24Bit = SSD1963_ReadPixel_24Bit;
        #endif
      }
    #endif
    #if LCD_DRIVER_HAS(ILI9325)
      else if (LCD_DriveIsILI9325())
      {
        ILI9325_Init_Sequential();
        pLCD_SetDirection = ILI9325_SetDirection;
        pLCD_SetWindow = ILI9325_SetWindow;
        #ifdef SCREEN_SHOT_TO_SD
          pLCD_ReadPixel_24Bit = ILI9325_ReadPixel_24Bit;
        #endif
      }
    #endif
    #if LCD_DRIVER_HAS(NT35310)
      else if (LCD_DriveIsNT35310())
      {
        NT35310_Init_Sequential();
        pLCD_SetDirection = NT35310_SetDirection;
        pLCD_SetWindow = NT35310_SetWindow;
        #ifdef SCREEN_SHOT_TO_SD
          pLCD_ReadPixel_24Bit = NT35310_ReadPixel_24Bit;
        #endif
      }
    #endif
    #if LCD_DRIVER_HAS(ST7796S)
      else if (LCD_DriveIsST7796S())
      {
        ST7796S_Init_Sequential();
        pLCD_SetDirection = ST7796S_SetDirection;
        pLCD_SetWindow = ST7796S_SetWindow;
        #ifdef SCREEN_SHOT_TO_SD
          pLCD_ReadPixel_24Bit = ST7796S_ReadPixel_24Bit;
        #endif
      }
    #endif
  #endif
}

void LCD_Init(void)
{
  LCD_HardwareConfig();
  LCD_Init_Sequential();
  LCD_RefreshDirection(0);
  GUI_Clear(BLACK);
  Delay_ms(120);

  #ifdef LCD_LED_PIN
    LCD_LED_Init();
    LCD_LED_On();
  #endif

  #ifdef STM32_HAS_FSMC
    LCD_DMA_Config();  // spi flash to fsmc lcd DMA channel configuration
  #endif
}

void LCD_RefreshDirection(uint8_t rotate)
{
  pLCD_SetDirection(rotate);
}

void LCD_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
  pLCD_SetWindow(sx, sy, ex, ey);
}
