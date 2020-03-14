#include "xpt2046.h"
#include "GPIO_Init.h"
#include "includes.h"
/***************************************** XPT2046 SPI 模式底层移植的接口********************************************/

//XPT2046 SPI related-using analog SPI  //XPT2046 SPI相关 - 使用模拟SPI
_SW_SPI xpt2046;

//Chip Select  //片选
void XPT2046_CS_Set(u8 level)
{
  SW_SPI_CS_Set(&xpt2046, level);
}

//Read and write functions  //读写函数
u8 XPT2046_ReadWriteByte(u8 TxData)
{
  return SW_SPI_Read_Write(&xpt2046, TxData);
}

//XPT2046 SPI and pen interrupt initialization  //XPT2046 SPI和笔中断初始化
void XPT2046_Init(void)
{
  //PA15-TPEN
  GPIO_InitSet(XPT2046_TPEN, MGPIO_MODE_IPN, 0);

  SW_SPI_Config(&xpt2046, _SPI_MODE3, 8, // 8bit
  XPT2046_CS,     //CS
  XPT2046_SCK,    //SCK
  XPT2046_MISO,   //MISO
  XPT2046_MOSI    //MOSI
  );
  XPT2046_CS_Set(1);
}

//Reading pen interrupt  //读笔中断
u8 XPT2046_Read_Pen(void)
{
  return GPIO_GetLevel(XPT2046_TPEN);
}
/******************************************************************************************************************/

//Read the converted AD value of XPT2046  //读取 XPT2046 转化好的AD值
u16 XPT2046_Read_AD(u8 CMD)
{
  u16 ADNum;
  XPT2046_CS_Set(0);

  XPT2046_ReadWriteByte(CMD);
  ADNum=XPT2046_ReadWriteByte(0xff);
  ADNum= ((ADNum)<<8) | XPT2046_ReadWriteByte(0xff);
  ADNum >>= 4;         //XPT2046 data is only 12 bits, discarding the lower four bits  //XPT2046数据只有12bits,舍弃低四位

  XPT2046_CS_Set(1);
  return ADNum;
}

#define READ_TIMES 5  //Read times  //读取次数
#define LOST_VAL 1    //Drop value  //丢弃值
u16 XPT2046_Average_AD(u8 CMD)
{
  u16 i, j;
  u16 buf[READ_TIMES];
  u16 sum=0;
  u16 temp;
  for(i=0; i<READ_TIMES; i++) buf[i] = XPT2046_Read_AD(CMD);
  for(i=0; i<READ_TIMES-1; i++)//Sort  //排序
  {
    for(j=i+1; j<READ_TIMES; j++)
    {
      if(buf[i] > buf[j]) //Ascending  //升序排列
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  for(i=LOST_VAL; i < READ_TIMES-LOST_VAL; i++) sum += buf[i];
  temp = sum/(READ_TIMES-2*LOST_VAL);
  return temp;
}


#define ERR_RANGE 50 //Tolerance scope  //误差范围
u16 XPT2046_Repeated_Compare_AD(u8 CMD)
{
  u16 ad1, ad2;
  ad1 = XPT2046_Average_AD(CMD);
  ad2 = XPT2046_Average_AD(CMD);

  if((ad2 <= ad1 && ad1 < ad2 + ERR_RANGE)
  || (ad1 <= ad2 && ad2 < ad1 + ERR_RANGE)) //The error before and after is less than ERR_RANGE  //前后两次误差小于 ERR_RANGE
  {
    return (ad1+ad2)/2;
  }
  else return 0;
}
