#include "LCD_Init.h"
#include "GPIO_Init.h"
#include "includes.h"


#ifdef LCD_LED_PIN
void LCD_LED_On()
{
  #ifdef LCD_LED_PWM_CHANNEL
    Set_LCD_Brightness(100);
  #else
    GPIO_SetLevel(LCD_LED_PIN, 1);
  #endif
}
void LCD_LED_Off()
{
  #ifdef LCD_LED_PWM_CHANNEL
    Set_LCD_Brightness(0);
  #else
    GPIO_SetLevel(LCD_LED_PIN, 0);
  #endif
}

#ifdef LCD_LED_PWM_CHANNEL
LCD_AUTO_DIM lcd_dim;
const uint32_t LCD_BRIGHTNESS[ITEM_BRIGHTNESS_NUM] = {
  LCD_5_PERCENT,
  LCD_10_PERCENT,
  LCD_20_PERCENT,
  LCD_30_PERCENT,
  LCD_40_PERCENT,
  LCD_50_PERCENT,
  LCD_60_PERCENT,
  LCD_70_PERCENT,
  LCD_80_PERCENT,
  LCD_90_PERCENT,
  LCD_100_PERCENT
};

const LABEL itemDimTime[ITEM_SECONDS_NUM] = {
  //item value text(only for custom value)
  LABEL_OFF,
  LABEL_5_SECONDS,
  LABEL_10_SECONDS,
  LABEL_30_SECONDS,
  LABEL_60_SECONDS,
  LABEL_120_SECONDS,
  LABEL_300_SECONDS,
  LABEL_CUSTOM_SECONDS
};

const uint32_t LCD_DIM_IDLE_TIME[ITEM_SECONDS_NUM] = {
  LCD_DIM_OFF,
  LCD_DIM_5_SECONDS,
  LCD_DIM_10_SECONDS,
  LCD_DIM_30_SECONDS,
  LCD_DIM_60_SECONDS,
  LCD_DIM_120_SECONDS,
  LCD_DIM_300_SECONDS,
  LCD_DIM_CUSTOM_SECONDS
};

void LCD_Dim_Idle_Timer_init()
{
  lcd_dim.idle_time_counter  = 0;
  lcd_dim._last_dim_state    = false;
  lcd_dim.idle_timer_reset   = false;
}

void LCD_Dim_Idle_Timer_Reset()
{
  if(infoSettings.lcd_idle_timer > LCD_DIM_OFF) {
    lcd_dim.idle_timer_reset= true;
  }
}

void LCD_Dim_Idle_Timer()
{
  if(infoSettings.lcd_idle_timer > LCD_DIM_OFF)
  {
    if(lcd_dim.idle_time_counter >= (LCD_DIM_IDLE_TIME[infoSettings.lcd_idle_timer] * 1000))
    {
      Set_LCD_Brightness(LCD_BRIGHTNESS[infoSettings.lcd_idle_brightness]);
      lcd_dim._last_dim_state= true;
    } else lcd_dim.idle_time_counter++;

    if(lcd_dim.idle_timer_reset)
    {
      if(lcd_dim._last_dim_state)
      {
        Set_LCD_Brightness(LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
        lcd_dim._last_dim_state = false;
      }

      lcd_dim.idle_timer_reset  = false;
      lcd_dim.idle_time_counter = 0;
    }
  }
}
#endif

void LCD_LED_Init(void)
{
  #ifdef LCD_LED_PWM_CHANNEL
    GPIO_InitSet(LCD_LED_PIN, MGPIO_MODE_AF_PP, LCD_LED_PIN_ALTERNATE);
    TIM_PWM_Init(LCD_LED_PWM_CHANNEL);
    LCD_Dim_Idle_Timer_init();
  #else
    LCD_LED_Off();
    GPIO_InitSet(LCD_LED_PIN, MGPIO_MODE_OUT_PP, 0);
  #endif
}
#endif

#if LCD_DRIVER_IS(RM68042)
// RM68042
void LCD_init_RGB(void)
{
  LCD_WR_REG(0X11);
  Delay_ms(20);
  LCD_WR_REG(0XD0);//VCI1  VCL  VGH  VGL DDVDH VREG1OUT power amplitude setting
  LCD_WR_DATA(0X07);
  LCD_WR_DATA(0X42);
  LCD_WR_DATA(0X1C);
  LCD_WR_REG(0XD1);//VCOMH VCOM_AC amplitude setting
  LCD_WR_DATA(0X00);
  LCD_WR_DATA(0X19);
  LCD_WR_DATA(0X16);
  LCD_WR_REG(0XD2);//Operational Amplifier Circuit Constant Current Adjust , charge pump frequency setting
  LCD_WR_DATA(0X01);
  LCD_WR_DATA(0X11);
  LCD_WR_REG(0XE4);
  LCD_WR_DATA(0X00A0);
  LCD_WR_REG(0XF3);
  LCD_WR_DATA(0X0000);
  LCD_WR_DATA(0X002A);
  LCD_WR_REG(0XC0);//REV SM GS
  LCD_WR_DATA(0X10);
  LCD_WR_DATA(0X3B);
  LCD_WR_DATA(0X00);
  LCD_WR_DATA(0X02);
  LCD_WR_DATA(0X11);
  LCD_WR_REG(0XC5);// Frame rate setting = 72HZ  when setting 0x03
  LCD_WR_DATA(0X03);
  LCD_WR_REG(0XC8);//Gamma setting
  LCD_WR_DATA(0X00);
  LCD_WR_DATA(0X35);
  LCD_WR_DATA(0X23);
  LCD_WR_DATA(0X07);
  LCD_WR_DATA(0X00);
  LCD_WR_DATA(0X04);
  LCD_WR_DATA(0X45);
  LCD_WR_DATA(0X53);
  LCD_WR_DATA(0X77);
  LCD_WR_DATA(0X70);
  LCD_WR_DATA(0X00);
  LCD_WR_DATA(0X04);
  LCD_WR_REG(0X20);//Exit invert mode
  LCD_WR_REG(0X36);
  LCD_WR_DATA(0X28);
  LCD_WR_REG(0X3A);
  LCD_WR_DATA(0X55);//16λģʽ
  Delay_ms(120);
  LCD_WR_REG(0X29);
}

#elif LCD_DRIVER_IS(ILI9488)
// ILI9488
void LCD_init_RGB(void)
{
  LCD_WR_REG(0xC0);
  LCD_WR_DATA(0x0c);
  LCD_WR_DATA(0x02);
  LCD_WR_REG(0xC1);
  LCD_WR_DATA(0x44);
  LCD_WR_REG(0xC5);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x16);
  LCD_WR_DATA(0x80);
  LCD_WR_REG(0x36);
  LCD_WR_DATA(0x28);
  LCD_WR_REG(0x3A); //Interface Mode Control
  LCD_WR_DATA(0x55);
  LCD_WR_REG(0XB0);  //Interface Mode Control
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB1);   //Frame rate 70HZ
  LCD_WR_DATA(0xB0);
  LCD_WR_REG(0xB4);
  LCD_WR_DATA(0x02);
  LCD_WR_REG(0xB6); //RGB/MCU Interface Control
  LCD_WR_DATA(0x02);
  LCD_WR_DATA(0x02);
  LCD_WR_REG(0xE9);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0XF7);
  LCD_WR_DATA(0xA9);
  LCD_WR_DATA(0x51);
  LCD_WR_DATA(0x2C);
  LCD_WR_DATA(0x82);
  LCD_WR_REG(0x11);
  Delay_ms(120);
  LCD_WR_REG(0x29);
}

#elif LCD_DRIVER_IS(ILI9341)
// ILI9341
void LCD_init_RGB(void)
{
  Delay_ms(50); // delay 50 ms

  LCD_WR_REG(0xCF);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xC1);
  LCD_WR_DATA(0X30);

  LCD_WR_REG(0xED);
  LCD_WR_DATA(0x64);
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0X12);
  LCD_WR_DATA(0X81);

  LCD_WR_REG(0xE8);   /// @diff :Driver timing control A
  LCD_WR_DATA(0x8A);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x78);

  LCD_WR_REG(0xEA);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);

  LCD_WR_REG(0xCB);
  LCD_WR_DATA(0x39);
  LCD_WR_DATA(0x2C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x34);
  LCD_WR_DATA(0x02);

  LCD_WR_REG(0xF7);
  LCD_WR_DATA(0x20);

  LCD_WR_REG(0xC0);    /// @diff Power control
  LCD_WR_DATA(0x25);   // VRH[5:0]

  LCD_WR_REG(0xC1);    /// @diff control
  LCD_WR_DATA(0x12);   // SAP[2:0];BT[3:0]

  LCD_WR_REG(0xC5);    /// @diff VCM control
  LCD_WR_DATA(0x33);
  LCD_WR_DATA(0x3C);

  LCD_WR_REG(0xC7);    ///@diff VCM control2
  LCD_WR_DATA(0x9A);

  LCD_WR_REG(0xB1);    /// @diff Frame Rate Control
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x15);

  LCD_WR_REG(0x3A);
  LCD_WR_DATA(0x55);

  LCD_WR_REG(0x36);    // Memory Access Control
  LCD_WR_DATA(0x68);

  LCD_WR_REG(0xB6);    // Display Function Control
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0xA2);

  LCD_WR_REG(0xF2);    // 3Gamma Function Disable
  LCD_WR_DATA(0x00);

  LCD_WR_REG(0x26);    // Gamma curve selected
  LCD_WR_DATA(0x01);

  LCD_WR_REG(0xE0);    /// @diff Set Gamma
  LCD_WR_DATA(0x1F);
  LCD_WR_DATA(0x1C);
  LCD_WR_DATA(0x1A);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x0F);
  LCD_WR_DATA(0x08);
  LCD_WR_DATA(0x47);
  LCD_WR_DATA(0xC8);
  LCD_WR_DATA(0x37);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x14);
  LCD_WR_DATA(0x05);
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0x08);
  LCD_WR_DATA(0x00);

  LCD_WR_REG(0XE1);    /// @diff Set Gamma
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(0x25);
  LCD_WR_DATA(0x04);
  LCD_WR_DATA(0x10);
  LCD_WR_DATA(0x07);
  LCD_WR_DATA(0x38);
  LCD_WR_DATA(0x48);
  LCD_WR_DATA(0x48);
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0x35);
  LCD_WR_DATA(0x37);
  LCD_WR_DATA(0x1F);

  LCD_WR_REG(0x2A);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x3f);

  LCD_WR_REG(0x2B);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xef);

  LCD_WR_REG(0x11); //Exit Sleep
  Delay_ms(120);
  LCD_WR_REG(0x29); //display on
}

#elif LCD_DRIVER_IS(ST7789)
// ST7789
void LCD_init_RGB(void)
{
 	LCD_WR_REG(0x11);
	Delay_ms(120); //Delay 120ms
	//------------------------------display and color format setting--------------------------------//
	LCD_WR_REG(0x36);
	LCD_WR_DATA(0x68);
	LCD_WR_REG(0x3a);
	LCD_WR_DATA(0x05);
	//--------------------------------ST7789V Frame rate setting----------------------------------//
	LCD_WR_REG(0xb2);
	LCD_WR_DATA(0x0c);
	LCD_WR_DATA(0x0c);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x33);
	LCD_WR_DATA(0x33);
	LCD_WR_REG(0xb7);
	LCD_WR_DATA(0x35);
	//---------------------------------ST7789V Power setting--------------------------------------//
	LCD_WR_REG(0xbb);
	LCD_WR_DATA(0x28);
	LCD_WR_REG(0xc0);
	LCD_WR_DATA(0x2c);
	LCD_WR_REG(0xc2);
	LCD_WR_DATA(0x01);
	LCD_WR_REG(0xc3);
	LCD_WR_DATA(0x0b);
	LCD_WR_REG(0xc4);
	LCD_WR_DATA(0x20);
	LCD_WR_REG(0xc6);
	LCD_WR_DATA(0x0f);
	LCD_WR_REG(0xd0);
	LCD_WR_DATA(0xa4);
	LCD_WR_DATA(0xa1);
	//--------------------------------ST7789V gamma setting---------------------------------------//
	LCD_WR_REG(0xe0);
	LCD_WR_DATA(0xd0);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x2a);
	LCD_WR_DATA(0x36);
	LCD_WR_DATA(0x55);
	LCD_WR_DATA(0x44);
	LCD_WR_DATA(0x3a);
	LCD_WR_DATA(0x0b);
	LCD_WR_DATA(0x06);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x20);
	LCD_WR_REG(0xe1);
	LCD_WR_DATA(0xd0);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x0a);
	LCD_WR_DATA(0x0b);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x34);
	LCD_WR_DATA(0x43);
	LCD_WR_DATA(0x4a);
	LCD_WR_DATA(0x2b);
	LCD_WR_DATA(0x1b);
	LCD_WR_DATA(0x1c);
	LCD_WR_DATA(0x22);
	LCD_WR_DATA(0x1f);
	LCD_WR_REG(0x29);
}

#elif LCD_DRIVER_IS(HX8558)
// HX8558
void LCD_init_RGB(void)
{
  Delay_ms(50); // delay 50 ms

  LCD_WR_REG(0xFE);                     //
  LCD_WR_REG(0xEF);
  LCD_WR_REG(0x3A);
  LCD_WR_DATA(5);
  LCD_WR_REG(0X36);
  LCD_WR_DATA(0x64);
  LCD_WR_REG(0xE8);
  LCD_WR_DATA(0x12);
  LCD_WR_DATA(0x22);
  LCD_WR_REG(0xE3);
  LCD_WR_DATA(1);
  LCD_WR_DATA(4);
  LCD_WR_REG(0xA5);
  LCD_WR_DATA(0x40);
  LCD_WR_DATA(0x40);
  LCD_WR_REG(0xA4);
  LCD_WR_DATA(0x44);
  LCD_WR_DATA(0x44);
  LCD_WR_REG(0xAB);
  LCD_WR_DATA(8);
  LCD_WR_REG(0xAA);
  LCD_WR_DATA(0x88);
  LCD_WR_DATA(0x88);
  LCD_WR_REG(0xAE);
  LCD_WR_DATA(0xB);
  LCD_WR_REG(0xAC);
  LCD_WR_DATA(0);
  LCD_WR_REG(0xAF);
  LCD_WR_DATA(0x77);
  LCD_WR_REG(0xAD);
  LCD_WR_DATA(0x77);

  LCD_WR_REG(0x2A);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0xEF);
  LCD_WR_REG(0x2B);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
  LCD_WR_DATA(1);
  LCD_WR_DATA(0x3F);

  LCD_WR_REG(0x2C);

  LCD_WR_REG(0xF0);
  LCD_WR_DATA(2);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
  LCD_WR_DATA(1);
  LCD_WR_DATA(1);
  LCD_WR_DATA(7);

  LCD_WR_REG(0xF1);
  LCD_WR_DATA(1);
  LCD_WR_DATA(3);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0x36);
  LCD_WR_DATA(41);
  LCD_WR_DATA(0x13);

  LCD_WR_REG(0xF2);
  LCD_WR_DATA(8);
  LCD_WR_DATA(6);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(3);
  LCD_WR_DATA(5);
  LCD_WR_DATA(0x34);

  LCD_WR_REG(0xF3);
  LCD_WR_DATA(0x16);
  LCD_WR_DATA(0xC);
  LCD_WR_DATA(0x5A);
  LCD_WR_DATA(4);
  LCD_WR_DATA(3);
  LCD_WR_DATA(0x69);

  LCD_WR_REG(0xF4);
  LCD_WR_DATA(0xD);
  LCD_WR_DATA(0x18);
  LCD_WR_DATA(0x15);
  LCD_WR_DATA(5);
  LCD_WR_DATA(5);
  LCD_WR_DATA(0);

  LCD_WR_REG(0xF5);
  LCD_WR_DATA(0xD);
  LCD_WR_DATA(0x18);
  LCD_WR_DATA(0x17);
  LCD_WR_DATA(0x35);
  LCD_WR_DATA(0x39);
  LCD_WR_DATA(0);
  LCD_WR_REG(0x11);

  Delay_ms(150);

  LCD_WR_REG(0x29);
  LCD_WR_REG(0x2C);
}

#elif LCD_DRIVER_IS(SSD1963)
// SSD1963  resolution max:864*480
#define SSD_HOR_RESOLUTION   LCD_WIDTH  // LCD width pixel
#define SSD_VER_RESOLUTION   LCD_HEIGHT // LCD height pixel

#define SSD_HT  (SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS (SSD_HOR_BACK_PORCH)
#define SSD_VT  (SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

void LCD_init_RGB(void)
{
  uint32_t LCDC_FPR;
  LCD_WR_REG(0xE2);  // Set PLL with OSC = 25MHz (hardware), 250Mhz < VC0 < 800Mhz
  LCD_WR_DATA(0x17); // M = 0x17 = 23, VCO = 25Mhz * (M + 1) = 25 * 24 = 600Mhz
  LCD_WR_DATA(0x04); // N = 0x04 = 4, PLL = VCO / (N + 1) = 600 / 5 = 120Mhz
  LCD_WR_DATA(0x54); // C[2] = 1, Effectuate the multiplier and divider value
  LCD_WR_REG(0xE0);  // Start PLL command
  LCD_WR_DATA(0x01); // enable PLL
  Delay_ms(10);
  LCD_WR_REG(0xE0);  // Start PLL command again
  LCD_WR_DATA(0x03); // now, use PLL output as system clock
  Delay_ms(10);
  LCD_WR_REG(0x01);  // Soft reset
  Delay_ms(100);
  LCDC_FPR = (SSD_DCLK_FREQUENCY * 1048576) / 120 -1; // DCLK Frequency = PLL * (LCDC_FPR + 1)/1048576, LCDC_FPR = (DCLK Frequency * 1048576) / PLL - 1
  LCD_WR_REG(0xE6);  // 12Mhz = 120Mhz * (LCDC_FPR + 1)/1048576, LCDC_FPR = 104856.6 = 0x019998
  LCD_WR_DATA((LCDC_FPR >> 16) & 0xFF);
  LCD_WR_DATA((LCDC_FPR >> 8) & 0xFF);
  LCD_WR_DATA(LCDC_FPR & 0xFF);
  LCD_WR_REG(0xB0);  // Set LCD mode
  LCD_WR_DATA(0x00); // 0x00: 16bits data, 0x20: 24bits data
  LCD_WR_DATA(0x00); // 0x00: TFT Mode
  LCD_WR_DATA((SSD_HOR_RESOLUTION - 1) >> 8); // LCD width pixel
  LCD_WR_DATA((SSD_HOR_RESOLUTION - 1) & 0xFF);
  LCD_WR_DATA((SSD_VER_RESOLUTION - 1) >> 8); // LCD height pixel
  LCD_WR_DATA((SSD_VER_RESOLUTION - 1) & 0xFF);
  LCD_WR_DATA(0x00); // RGB format
  LCD_WR_REG(0xB4);  // Set horizontal period
  LCD_WR_DATA((SSD_HT - 1) >> 8); // Horizontal total period (display + non-display) in pixel clock
  LCD_WR_DATA(SSD_HT - 1);
  LCD_WR_DATA(SSD_HPS >> 8); // Non-display period between the start of the horizontal sync (LLINE) signal and the first display data
  LCD_WR_DATA(SSD_HPS);
  LCD_WR_DATA(SSD_HOR_PULSE_WIDTH - 1); // horizontal sync pulse width (LLINE) in pixel clock
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB6);  // Set vertical period
  LCD_WR_DATA((SSD_VT - 1) >> 8);
  LCD_WR_DATA(SSD_VT - 1);
  LCD_WR_DATA(SSD_VPS >> 8);
  LCD_WR_DATA(SSD_VPS);
  LCD_WR_DATA(SSD_VER_FRONT_PORCH - 1);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xF0);  // Set pixel data interface format
  LCD_WR_DATA(0x03); // 16-bit(565 format) data for 16bpp
  LCD_WR_REG(0x29);  // Set display on

  LCD_WR_REG(0x36);  // Set address mode
  LCD_WR_DATA(0x00);
}

#endif

u16 LCD_ReadID(void)
{
  u16 id = 0;
  LCD_WR_REG(0XD3);
  id = LCD_RD_DATA();	//dummy read
  id = LCD_RD_DATA();
  id = LCD_RD_DATA();
  id <<= 8;

  id |= LCD_RD_DATA();
  return id;
}

void LCD_RefreshDirection(void)
{
  LCD_WR_REG(0X36);
  LCD_WR_DATA(infoSettings.rotate_ui ? TFTLCD_180_DEGREE_REG_VALUE : TFTLCD_0_DEGREE_REG_VALUE);
}

void LCD_Init(void)
{
  LCD_HardwareConfig();
  LCD_init_RGB();
  GUI_Clear(BLACK);
  Delay_ms(120);

#ifdef LCD_LED_PIN
  LCD_LED_Init();
  LCD_LED_On();
#endif

#ifdef STM32_HAS_FSMC
  LCD_DMA_Config();  //spi flash to fsmc lcd DMA channel configuration
#endif
}
