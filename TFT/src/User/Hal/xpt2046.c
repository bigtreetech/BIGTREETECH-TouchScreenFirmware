#include "xpt2046.h"
#include "GPIO_Init.h"
#include "sw_spi.h"
#include "includes.h"  // for HW_SPI_TOUCH, XPT2046_SPI (added for MKS_TFT35_V1_0 support)

/***************************************** XPT2046 SPI mode low-level porting interface********************************************/

//XPT2046 SPI related-using analog SPI  //XPT2046 SPIRelated-Using Analog SPI
_SW_SPI xpt2046;

//Chip Select  //Chip Select
void XPT2046_CS_Set(uint8_t level)
{
#ifdef HW_SPI_TOUCH  // added for MKS_TFT35_V1_0 support
  GPIO_SetLevel(XPT2046_CS, level);
  W25Qxx_SPI_CS_Set(1);
#else
  SW_SPI_CS_Set(&xpt2046, level);
#endif
}

//Read and write functions  //Read and write functions
uint8_t XPT2046_ReadWriteByte(uint8_t TxData)
{
#ifdef HW_SPI_TOUCH  // added for MKS_TFT35_V1_0 support
  return SPI_Read_Write(XPT2046_SPI, TxData);
#else
  return SW_SPI_Read_Write(&xpt2046, TxData);
#endif
}

//XPT2046 SPI and pen interrupt initialization  //XPT2046 SPI and pen interrupt initialization
void XPT2046_Init(void)
{
  //PA15-TPEN
  GPIO_InitSet(XPT2046_TPEN, MGPIO_MODE_IPN, 0);

#ifdef HW_SPI_TOUCH  // added for MKS_TFT35_V1_0 support
  GPIO_InitSet(XPT2046_CS, MGPIO_MODE_OUT_PP, 0);
#else
  SW_SPI_Config(&xpt2046, _SPI_MODE3, 8,  // 8bit
    XPT2046_CS,    //CS
    XPT2046_SCK,   //SCK
    XPT2046_MISO,  //MISO
    XPT2046_MOSI   //MOSI
  );
#endif

  XPT2046_CS_Set(1);
}

//Reading pen interrupt  //Reading pen interrupt
uint8_t XPT2046_Read_Pen(void)
{
  return GPIO_GetLevel(XPT2046_TPEN);
}
/******************************************************************************************************************/

//Read the converted AD value of XPT2046  //Read the converted AD value of XPT2046
uint16_t XPT2046_Read_AD(uint8_t CMD)
{
  uint16_t ADNum;
  XPT2046_CS_Set(0);

#ifdef MKS_TFT35_V1_0                // added for MKS_TFT35_V1_0 support
  SPI_Protocol_Init(W25Qxx_SPI, 5);  // max. ADS7843 SPI clock is 2 MHz, switch to low SPI speed when sampling touch screen, 4-7 works
  // APB2 max frequency 84 MHz
  // 0:fPCLK/2 42MHz, 1:fPCLK/4 21MHz, 2:fPCLK/8 10.5MHz, 3:fPCLK/16 5.3MHz, 4:fPCLK/32 2.6MHz, 5:fPCLK/64 1.3MHz, 6:fPCLK/128 0.7 MHz, 7:fPCLK/256 0.33MHz
  // MKS Touchscreen source uses: SPI1_SetSpeed(SPI_BaudRatePrescaler_256)
#endif

  XPT2046_ReadWriteByte(CMD);
  ADNum=XPT2046_ReadWriteByte(0xff);
  ADNum= ((ADNum) << 8) | XPT2046_ReadWriteByte(0xff);

#ifdef MKS_TFT35_V1_0                           // added for MKS_TFT35_V1_0 support
  SPI_Protocol_Init(W25Qxx_SPI, W25Qxx_SPEED);  // switch back to high speed SPI
#endif

  ADNum >>= 4;  //XPT2046 data is only 12 bits, discarding the lower four bits

  XPT2046_CS_Set(1);
  return ADNum;
}

#define READ_TIMES 5  //Read times  //Read times
#define LOST_VAL   1  //Drop value  //Drop value

uint16_t XPT2046_Average_AD(uint8_t CMD)
{
  uint16_t i, j;
  uint16_t buf[READ_TIMES];
  uint16_t sum = 0;
  uint16_t temp;
  for (i = 0; i < READ_TIMES; i++) buf[i] = XPT2046_Read_AD(CMD);
  for (i = 0; i < READ_TIMES - 1; i++)  //Sort  // Sort
  {
    for (j = i + 1; j < READ_TIMES; j++)
    {
      if (buf[i] > buf[j])  //Ascending
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++) sum += buf[i];
  temp = sum / (READ_TIMES - 2 * LOST_VAL);
  return temp;
}

#define ERR_RANGE 50  //Tolerance scope

uint16_t XPT2046_Repeated_Compare_AD(uint8_t CMD)
{
  uint16_t ad1, ad2;
  ad1 = XPT2046_Average_AD(CMD);
  ad2 = XPT2046_Average_AD(CMD);

  if ((ad2 <= ad1 && ad1 < ad2 + ERR_RANGE)
   || (ad1 <= ad2 && ad2 < ad1 + ERR_RANGE))  //The error before and after is less than ERR_RANGE  //Two errors before and after ERR_RANGE
  {
    return (ad1 + ad2) / 2;
  }
  else return 0;
}
