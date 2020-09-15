/**
 * For MKSTFT35 LCD i2C EEPROM support
 * Currently hard coded for I2C1 device
 * 
 */
//Dont include code if not needed

#if defined(MKS_35_V1_0) || defined(I2C_EEPROM) 

#include "i2c_eeprom.h"
#include "my_misc.h"
#include "GPIO_Init.h"
#include "variants.h"

uint8_t buffing[EEPROM_PAGESIZE+2]={0};

u8 AT24CXX_WriteData(uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);

void I2C1_init(uint32_t I2C_baudrate)
{
	I2C_InitTypeDef I2C_InitStructure;
	I2C_DeInit(I2C1); // Deinit I2C1 
	// configure I2C1 
	I2C_InitStructure.I2C_ClockSpeed 			= I2C_baudrate;		// 100kHz
	I2C_InitStructure.I2C_Mode 					= I2C_Mode_I2C;		// I2C mode
	I2C_InitStructure.I2C_DutyCycle 			= I2C_DutyCycle_2;	// 50% duty cycle --> standard
	I2C_InitStructure.I2C_OwnAddress1 			= 0x00;				// own address, not relevant in master mode
	I2C_InitStructure.I2C_Ack 					= I2C_Ack_Enable;	// Enable acknowledge when reading (can be changed later on)
	I2C_InitStructure.I2C_AcknowledgedAddress 	= I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
	I2C_Init(I2C1, &I2C_InitStructure);			// init I2Cx
	I2C_Cmd(I2C1, ENABLE); 	// Enable I2C1
	I2C_StretchClockCmd(I2C1,ENABLE);
}

void I2C_GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);		// Enable GPIO clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);	// Enable I2C clock
	
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);	// SCL
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1); // SDA

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_7 | GPIO_Pin_6; 	// we are going to use PB6 and PB9
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;				// set pins to alternate function
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;			// set GPIO speed
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_OD;			// set output to open drain --> the line has to be only pulled low, not driven high
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;				// enable pull up resistors
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	// Connect I2Cx pins to AF  
	
}
/*===============================================================================================================*/
										/* I2C_Write_Byte */
/*===============================================================================================================*/
void I2C_Write_Byte(I2C_TypeDef* I2Cx, uint8_t reg_addr , uint8_t reg_data, uint8_t dev_Addr)
{
	
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));/* While the bus is busy */


	I2C_GenerateSTART(I2Cx, ENABLE);/* Send I2C1 START condition */
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
GPIO_SetLevel(PB4, 0);
	
	I2C_Send7bitAddress(I2Cx, dev_Addr , I2C_Direction_Transmitter);/* Send Device slave Address for write */
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
GPIO_SetLevel(PA8, 0);

	I2C_SendData(I2Cx, reg_addr);/* Send the Device internal register address */
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));


	I2C_SendData(I2Cx, reg_data);/* Send I2C1 EEPROM data */
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_AcknowledgeConfig(I2Cx, DISABLE);/* Disable Acknowledgement */	
	Delay_us(1);/* Generate 12.285uS delay which is required*/

	I2C_GenerateSTOP(I2Cx, ENABLE);/* Send I2C1 STOP Condition */
	Delay_us(1);/* Generate 12.285uS delay which is required*/

	I2C_AcknowledgeConfig(I2Cx,ENABLE);/* Enable Acknowledgement to be ready for another reception */
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	GPIO_SetLevel(PA8, 1);
	GPIO_SetLevel(PB4, 1);
}
/*===============================================================================================================*/
										/* I2C_Read_Byte */
/*===============================================================================================================*/
uint8_t I2C_Read_Byte(I2C_TypeDef* I2Cx, uint8_t reg_addr, uint8_t dev_Addr)
{
	char Red_reg;
	/*------------------------------ Start Dummy write ----------------------------------------*/


	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));/* While the bus is busy */

	I2C_GenerateSTART(I2Cx, ENABLE);/* Send I2C1 START condition */
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2Cx, dev_Addr, I2C_Direction_Transmitter);/* Send Device slave Address for write */
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2Cx, reg_addr);/* Send the Device internal register address */
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	/*------------------------------- Dummy write End -----------------------------------------*/
	/*------------------------------ Random Read Start ----------------------------------------*/

	I2C_GenerateSTART(I2Cx, ENABLE);/* Send STRAT condition a second time called repeated start*/
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2Cx, (dev_Addr | 0x01) , I2C_Direction_Receiver);/* Send Device slave Address for read */
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	/* Test on I2C1 EV7 and clear it */ /* Check for BUSY, MSL and RXNE flags =(EV7)*/
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

	Red_reg = I2C_ReceiveData(I2Cx);

	I2C_AcknowledgeConfig(I2Cx, DISABLE);/* Disable Acknowledgement */
	Delay_us(1);/* Generate 12.285uS delay which is required*/
	I2C_GenerateSTOP(I2Cx, ENABLE);/* Send I2C1 STOP Condition */
	Delay_us(1);/* Generate 12.285uS delay which is required*/
	//I2C_AcknowledgeConfig(I2C1,ENABLE);/* Enable Acknowledgement to be ready for another reception */


	return Red_reg;/*Return data which is to be read from device*/
}

//new defines


//Debug Code
/*
void do_dump()
{
     int temp=0;
    char buff[30];
    I2C_GPIO_Config();
    I2C1_init(I2C_SPEED);
     GUI_DispString(0,0,"I2C Read       ");
    //GPIO_SetLevel(PB4, ! GPIO_GetLevel(PB4));
    int address=0;
    GUI_SetColor(RED);
    for (int read_amount=0;read_amount<1;read_amount++)
    {
        for (int row=1;row<11;row++)
        {
            for (int col=0;col<8;col++)
            {
                temp=I2C_Read_Byte(I2C_PORT,address,0xa0);
                sprintf(buff,"0x%02x",temp);
                GUI_DispString(((BYTE_WIDTH*col)*5),(BYTE_HEIGHT*row)+2,buff);
               // Delay_ms(5);
                address++;
                if (address>EEPROM_SIZE)
                break;
            }
        
        }
        Delay_ms(2000); 
        }
}
*/

u8 I2C_Write(I2C_TypeDef* I2Cx, const uint8_t* buf, uint8_t reg,  uint32_t nbyte, uint8_t SlaveAddress)
{
    __IO uint32_t Timeout = 0;

    /* Enable Error IT (used in all modes: DMA, Polling and Interrupts */
    //    I2Cx->CR2 |= I2C_IT_ERR;

    if (nbyte)
      {
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

	// Intiate Start Sequence

	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

	// Send Address  EV5

	I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	// EV6 - write reg 
    I2C_SendData(I2Cx,reg);
	while(!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF));
	// Write first byte EV8_1
	if (nbyte>16)
	{
		GUI_DispString(200,0,"large byte write");
	}

	I2C_SendData(I2Cx, *buf++);

	while (--nbyte) {

	  // wait on BTF

	  while(!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF));  
	  I2C_SendData(I2Cx, *buf++);
	}

	while(!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF));  
	I2C_GenerateSTOP(I2Cx, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
      }
    return 0;
 errReturn:
    return 1;
}


/* Debug Code
void disp_buff()
{
   char text_buf[30];
  int index=0;
  int row;
  for (row=1;row<7;row++)
      {
        for (int col=0;col<8;col++)
           {
              sprintf(text_buf,"0x%02x",buffing[index]);
              GUI_DispString(((BYTE_WIDTH*col)*5),(BYTE_HEIGHT*row)+2,text_buf);
            index++;
            } 
      }  
#if 0
  for (int i=0;i<32-1;i++)
  {
	//buffing="testingtesting";
    #if 1
    buffing[i+4]=0xff;
	  buffing[i+3]=0xDE;
	  buffing[i+2]=0xAD;
	  buffing[i+1]=0xBE;
	  buffing[i+0]=0xEF;
    #else
    buffing[i+0]=0xff;
	  buffing[i+1]=0xDE;
	  buffing[i+2]=0xAD;
	  buffing[i+3]=0xBE;
	  buffing[i+4]=0xEF;
    #endif
    
  // buffing[i]=0;
  }
  #else
  for (int i=0;i<32;i++)
  {
    buffing[i]=i;
  }
  #endif
  index=0;
  for (row=7;row<13;row++)
      {
        for (int col=0;col<8;col++)
           {
              sprintf(text_buf,"0x%02x",buffing[index]);
              GUI_SetColor(RED);
              GUI_DispString(((BYTE_WIDTH*col)*5),(BYTE_HEIGHT*row)+2,text_buf);
            index++;
            } 
      }
}
*/
void EEPROM_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t* NumByteToRead)
{

	//uint8_t I2C_Read_Byte(I2C_TypeDef* I2Cx, uint8_t reg_addr, uint8_t dev_Addr)
	*pBuffer = I2C_Read_Byte(I2C1,ReadAddr,AT24CXX_ADDRESS);
	#if 0
	while (--NumByteToRead)
	{
		*pBuffer = I2C_Read_Byte(I2C1,ReadAddr+NumByteToRead,AT24CXX_ADDRESS);
		pBuffer++;
		//NumByteToRead;
	}
	#else
	/* Debug Code
    u8 text_buff[50];
	snprintf(text_buff,50,"mem addy %x buff size %x",ReadAddr,NumByteToRead);
	GUI_DispString(150,0,text_buff);
    */
	for (int i=0;i<NumByteToRead;i++)
	{
		*pBuffer = I2C_Read_Byte(I2C1,ReadAddr+i,AT24CXX_ADDRESS);
		pBuffer++;
		//NumByteToRead;
	}
	#endif
}

/* Debug Code
void test_buff()
{

 
//uint32_t EEPROM_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t* NumByteToRead)
EEPROM_ReadBuffer(buffing,0,32);
GUI_DispString(0,0,"read i2c done");
disp_buff();
//u8 AT24CXX_WriteData(uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize)
AT24CXX_WriteData(0x0,buffing,32);

//Status I2C_Write(I2C_TypeDef* I2Cx, const uint8_t* buf,  uint32_t nbyte, uint8_t SlaveAddress)
//I2C_Write(I2C1,buffing,16,0xa0);
//EEPROM_WritePage(buffing,0,EEPROM_PAGESIZE);

while(1);
}
*/

void EEPROM_FlashRead(u8 *data, u16 len)
{
	EEPROM_ReadBuffer(data,0,(u16)len);
}
void  EEPROM_FlashWrite(u8 *data, u16 len)
{
	EEPROM_WriteData(0x0,data,(u16)len);
}


void i2C_Init_EEPROM()
{
    I2C_GPIO_Config();
    I2C1_init(I2C_SPEED);
}

u8 I2Cx_WriteMultiple(uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length)
{
  u8 status = EEPROM_OK;
  
I2C_Write(I2C1,Buffer,Reg, Length,Addr);
  /* Check the communication status */
  if(status != EEPROM_OK)
  {
    /* Re-Initiaize the I2C Bus */
   // I2Cx_Error(Addr);
  }
  return status;
}


u8 EEPROM_PageWrite(uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize)
{
	u8 status = EEPROM_OK;
	
	status = I2Cx_WriteMultiple((AT24CXX_ADDRESS+((MemAddress/256)<<1)),MemAddress,AT24CXX_MEMADD_SIZE, pBuffer,BufferSize); 
/*  Debug Code
	u8 text_buff[50];
	snprintf(text_buff,50,"mem addy %x buff size %x",MemAddress,BufferSize);
	GUI_DispString(150,0,text_buff);
	Delay_us(3000);   */
	Delay_us(3000);
	return status;
}




u8 EEPROM_WriteData(uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
	u8 err = EEPROM_OK;

	Addr = MemAddress % AT24CXX_PAGE_SIZE;
	count = AT24CXX_PAGE_SIZE - Addr;
	NumOfPage =  BufferSize / AT24CXX_PAGE_SIZE;
	NumOfSingle = BufferSize % AT24CXX_PAGE_SIZE;
 
	/* If WriteAddr is I2C_PageSize aligned  */
	if(Addr == 0) 
	{
		/* If NumByteToWrite < I2C_PageSize */
		if(NumOfPage == 0) 
		{
			return (EEPROM_PageWrite(MemAddress,pBuffer,BufferSize));
		}
		/* If NumByteToWrite > I2C_PageSize */
		else  
		{
			while(NumOfPage--)
			{
				err = EEPROM_PageWrite(MemAddress,pBuffer,AT24CXX_PAGE_SIZE);
				if(err != EEPROM_OK)
					return err; 
				MemAddress +=  AT24CXX_PAGE_SIZE;
				pBuffer += AT24CXX_PAGE_SIZE;
			}

			if(NumOfSingle!=0)
			{
				return (EEPROM_PageWrite(MemAddress,pBuffer,NumOfSingle)); 
			}
		}
	}
	  /* If WriteAddr is not I2C_PageSize aligned  */
	else 
	{
		/* If NumByteToWrite < I2C_PageSize */
		if(BufferSize <= count) 
		{
			return (EEPROM_PageWrite(MemAddress,pBuffer,NumOfSingle));
		}
		/* If NumByteToWrite > I2C_PageSize */
		else
		{
			BufferSize -= count;
			NumOfPage =  BufferSize / AT24CXX_PAGE_SIZE;
			NumOfSingle = BufferSize % AT24CXX_PAGE_SIZE;	
		  
			err = EEPROM_PageWrite(MemAddress,pBuffer,count);
			if(err != EEPROM_OK)
				return err; 
			MemAddress += count;
			pBuffer += count;
		  
			while(NumOfPage--)
			{
				err = EEPROM_PageWrite(MemAddress,pBuffer,AT24CXX_PAGE_SIZE);
				if(err != EEPROM_OK)
					return err;
				MemAddress +=  AT24CXX_PAGE_SIZE;
				pBuffer += AT24CXX_PAGE_SIZE;  
			}
			
			if(NumOfSingle != 0)
			{
				return (EEPROM_PageWrite(MemAddress,pBuffer,NumOfSingle)); 
			}
		}
	}

	return err;
}

#endif //end of dont include 