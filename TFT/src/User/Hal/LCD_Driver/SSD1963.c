#include "includes.h"

#if LCD_DRIVER_HAS(SSD1963)

  #include "SSD1963.h"
  // SSD1963  resolution max:864*480
  #define SSD_HOR_RESOLUTION LCD_HARDWARE_WIDTH   // LCD width pixel
  #define SSD_VER_RESOLUTION LCD_HARDWARE_HEIGHT  // LCD height pixel

  #define SSD_HT  (SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
  #define SSD_HPS (SSD_HOR_BACK_PORCH)
  #define SSD_VT  (SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
  #define SSD_VPS (SSD_VER_BACK_PORCH)

  void SSD1963_Init_Sequential(void)
  {
    uint32_t LCDC_FPR;
    LCD_WR_REG(0xE2);   // Set PLL with OSC = 25MHz (hardware), 250Mhz < VC0 < 800Mhz
    LCD_WR_DATA(0x17);  // M = 0x17 = 23, VCO = 25Mhz * (M + 1) = 25 * 24 = 600Mhz
    LCD_WR_DATA(0x04);  // N = 0x04 = 4, PLL = VCO / (N + 1) = 600 / 5 = 120Mhz
    LCD_WR_DATA(0x54);  // C[2] = 1, Effectuate the multiplier and divider value
    LCD_WR_REG(0xE0);   // Start PLL command
    LCD_WR_DATA(0x01);  // enable PLL
    Delay_ms(10);
    LCD_WR_REG(0xE0);   // Start PLL command again
    LCD_WR_DATA(0x03);  // now, use PLL output as system clock
    Delay_ms(10);
    LCD_WR_REG(0x01);   // Soft reset
    Delay_ms(100);
    LCDC_FPR = (SSD_DCLK_FREQUENCY * 1048576) / 120 -1;  // DCLK Frequency = PLL * (LCDC_FPR + 1)/1048576, LCDC_FPR = (DCLK Frequency * 1048576) / PLL - 1
    LCD_WR_REG(0xE6);   // 12Mhz = 120Mhz * (LCDC_FPR + 1)/1048576, LCDC_FPR = 104856.6 = 0x019998
    LCD_WR_DATA((LCDC_FPR >> 16) & 0xFF);
    LCD_WR_DATA((LCDC_FPR >> 8) & 0xFF);
    LCD_WR_DATA(LCDC_FPR & 0xFF);
    LCD_WR_REG(0xB0);   // Set LCD mode
    LCD_WR_DATA(0x00);  // 0x00: 16bits data, 0x20: 24bits data
    LCD_WR_DATA(0x00);  // 0x00: TFT Mode
    LCD_WR_DATA((SSD_HOR_RESOLUTION - 1) >> 8);  // LCD width pixel
    LCD_WR_DATA((SSD_HOR_RESOLUTION - 1) & 0xFF);
    LCD_WR_DATA((SSD_VER_RESOLUTION - 1) >> 8);  // LCD height pixel
    LCD_WR_DATA((SSD_VER_RESOLUTION - 1) & 0xFF);
    LCD_WR_DATA(0x00);  // RGB format
    LCD_WR_REG(0xB4);   // Set horizontal period
    LCD_WR_DATA((SSD_HT - 1) >> 8);  // Horizontal total period (display + non-display) in pixel clock
    LCD_WR_DATA(SSD_HT - 1);
    LCD_WR_DATA(SSD_HPS >> 8);  // Non-display period between the start of the horizontal sync (LLINE) signal and the first display data
    LCD_WR_DATA(SSD_HPS);
    LCD_WR_DATA(SSD_HOR_PULSE_WIDTH - 1);  // horizontal sync pulse width (LLINE) in pixel clock
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xB6);   // Set vertical period
    LCD_WR_DATA((SSD_VT - 1) >> 8);
    LCD_WR_DATA(SSD_VT - 1);
    LCD_WR_DATA(SSD_VPS >> 8);
    LCD_WR_DATA(SSD_VPS);
    LCD_WR_DATA(SSD_VER_FRONT_PORCH - 1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xF0);   // Set pixel data interface format
    LCD_WR_DATA(0x03);  // 16-bit(565 format) data for 16bpp
    LCD_WR_REG(0xBC);   // postprocessor for contrast/brightness/saturation.
    LCD_WR_DATA(0x34);  // Contrast value (0-127). Set to 52 to reduce banding/flickering.
    LCD_WR_DATA(0x77);  // Brightness value (0-127). Set to 119 to reduce banding/flickering.
    LCD_WR_DATA(0x48);  // Saturation value (0-127).
    LCD_WR_DATA(0x01);  // Enable/disable the postprocessor for contrast/brightness/saturation (1-0).
    LCD_WR_REG(0x29);   // Set display on

    LCD_WR_REG(0x36);   // Set address mode
    LCD_WR_DATA(0x00);
  }

  void SSD1963_SetDirection(uint8_t rotate)
  {
    LCD_WR_REG(0X36);
  
    #ifdef PORTRAIT_MODE
      LCD_WR_DATA(rotate ? SSD1963_270_DEGREE_REG_VALUE : SSD1963_90_DEGREE_REG_VALUE);
    #else
      LCD_WR_DATA(rotate ? SSD1963_180_DEGREE_REG_VALUE : SSD1963_0_DEGREE_REG_VALUE);
    #endif
  }

  void SSD1963_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
  {
    LCD_WR_REG(SSD1963_CMD_SET_X);
    LCD_WR_DATA(sx>>8);LCD_WR_DATA(sx&0xFF);
    LCD_WR_DATA(ex>>8);LCD_WR_DATA(ex&0xFF);
    LCD_WR_REG(SSD1963_CMD_SET_Y);
    LCD_WR_DATA(sy>>8);LCD_WR_DATA(sy&0xFF);
    LCD_WR_DATA(ey>>8);LCD_WR_DATA(ey&0xFF);
    LCD_WR_REG(0x2C);  // Ready to write memory
  }

  uint32_t SSD1963_ReadPixel_24Bit(int16_t x, int16_t y)
  {
    LCD_SetWindow(x, y, x, y);
    LCD_WR_REG(0X2E);
    Delay_us(1);

    GUI_COLOR pix;
    pix.color = LCD_RD_DATA();
    return (pix.RGB.r << 19) | (pix.RGB.g << 10) | (pix.RGB.b << 3);
  }

#endif  // LCD_DRIVER_HAS(SSD1963)
