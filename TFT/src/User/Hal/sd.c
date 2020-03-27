#include "variants.h"

#ifdef SD_SPI_SUPPORT
#include "sd.h"
#include "GPIO_Init.h"

u8  SD_Type=0;  //SD��������

/***************************************** SD SPI ģʽ�ײ���ֲ�Ľӿ�***********************************************/
//#define SD_SPI _SPI1
//#define SD_LOW_SPEED  7
//#define SD_HIGH_SPEED 1

//��д����
u8 SD_SPI_Read_Write_Byte( u8 data)
{
  return SPI_Read_Write(SD_SPI,data);
}

//Ƭѡ
void SD_SPI_CS_Set(u8 level)
{
  SPI_CS_Set(SD_SPI,level);
}

#ifdef SD_CD_PIN
//SD_CD insert detect
void SD_CD_WP_Init(void)
{
  GPIO_InitSet(SD_CD_PIN, MGPIO_MODE_IPU, 0);
}
#endif

u8 SD_CD_Inserted(void)
{
#ifdef SD_CD_PIN
  return !GPIO_GetLevel(SD_CD_PIN);
#else
  return 0;
#endif
}

//��ʼ��
void SD_SPI_Init(void)
{
#ifdef SD_CD_PIN
  SD_CD_WP_Init();
#endif
  SPI_Config(SD_SPI);
  SD_SPI_CS_Set(1);
}

void SD_DeInit(void)
{
  SPI_DeConfig(SD_SPI);
}

//��ʼ��ʱ��Ҫ����
void SD_SetLowSpeed(void)
{
  SPI_Protocol_Init(SD_SPI, SD_LOW_SPEED);
}

//��������ʱʹ�ø���
void SD_SetHighSpeed(void)
{
  SPI_Protocol_Init(SD_SPI, SD_HIGH_SPEED);
}
/******************************************************************************************************************/


/************************************************************************************
**ȡ��ѡ��,�ͷ�SPI����
*************************************************************************************/
void SD_Cancel_CS(void)
{
  SD_SPI_CS_Set(1);
  SD_SPI_Read_Write_Byte(0xff);//�ṩ�����8��ʱ��
}

/************************************************************************************
**ѡ��sd��,���ҵȴ���׼��OK
**����ֵ:0,�ɹ�;1,ʧ��;
*************************************************************************************/
u8 SD_Select(void)
{
  SD_SPI_CS_Set(0);
  if(SD_Wait_Ready()==0)return 0;//�ȴ��ɹ�
  SD_Cancel_CS();
  return 1;//�ȴ�ʧ��
}


/************************************************************************************
**�ȴ���׼����
**����ֵ:0,׼������;����,�������
**************************************************************************************/
u8 SD_Wait_Ready(void)
{
  u32 t=0;
  do
  {
    if(SD_SPI_Read_Write_Byte(0XFF)==0XFF)return 0;//OK
    t++;
  }while(t<0XFFFFFF);//�ȴ�
  return 1;
}


/************************************************************************************
**�ȴ�SD����Ӧ
**Response:Ҫ�õ��Ļ�Ӧֵ
**����ֵ:0,�ɹ��õ��˸û�Ӧֵ
**    ����,�õ���Ӧֵʧ��
*************************************************************************************/
u8 SD_Get_Ack(u8 Response)
{
  u16 Count=0xFFFF;//�ȴ�����
  while ((SD_SPI_Read_Write_Byte(0XFF)!=Response) && Count)  Count--; //�ȴ��õ�׼ȷ�Ļ�Ӧ

  if (Count==0)
    return SD_RESPONSE_FAILURE;//�õ���Ӧʧ��
  else
    return SD_RESPONSE_NO_ERROR;//��ȷ��Ӧ
}


/*************************************************************************************
**��sd����ȡһ�����ݰ�������
**buf:���ݻ�����
**len:Ҫ��ȡ�����ݳ���.
**����ֵ:0,�ɹ�;����,ʧ��;
****************************************************************************************/
u8 SD_RecvData(u8*buf,u16 len)
{
  if(SD_Get_Ack(0xFE))return 1;//�ȴ�SD������������ʼ����0xFE
  while(len--)//��ʼ��������
  {
    *buf=SD_SPI_Read_Write_Byte(0xFF);
    buf++;
  }
  //������2��αCRC��dummy CRC��
  SD_SPI_Read_Write_Byte(0xFF);
  SD_SPI_Read_Write_Byte(0xFF);
  return 0;//��ȡ�ɹ�
}



/************************************************************************************
**��sd��д��һ�����ݰ������� 512�ֽ�
**buf:���ݻ�����
**cmd:ָ��
**����ֵ:0,�ɹ�;����,ʧ��;
*************************************************************************************/
u8 SD_Send_Data(u8*buf,u8 cmd)
{
  u16 t;
  if(SD_Wait_Ready())  return 1;  //�ȴ�׼��ʧЧ
  SD_SPI_Read_Write_Byte(cmd);
  if(cmd!=0XFD)//���ǽ���ָ��
  {
    for(t=0;t<512;t++)SD_SPI_Read_Write_Byte(buf[t]);

    SD_SPI_Read_Write_Byte(0xFF); //����crc
    SD_SPI_Read_Write_Byte(0xFF);
    t = SD_SPI_Read_Write_Byte(0xFF); //������Ӧ
    if((t&0x1F) != 0x05)return 2;   //��Ӧ����
  }
  return 0;//д��ɹ�
}



/*************************************************************************************
**��SD������һ������
**����: u8 cmd   ����
**      u32 arg  �������
**      u8 crc   crcУ��ֵ
**����ֵ:SD�����ص���Ӧ
***************************************************************************************/
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
  u8 r1;
  u8 Retry=0;
  SD_Cancel_CS();              //ȡ���ϴ�Ƭѡ
  if(SD_Select())  return 0XFF;//ƬѡʧЧ
  //����
  SD_SPI_Read_Write_Byte(cmd | 0x40);//�ֱ�д������
  SD_SPI_Read_Write_Byte(arg >> 24);
  SD_SPI_Read_Write_Byte(arg >> 16);
  SD_SPI_Read_Write_Byte(arg >> 8);
  SD_SPI_Read_Write_Byte(arg);
  SD_SPI_Read_Write_Byte(crc);
  if(cmd==CMD12)  SD_SPI_Read_Write_Byte(0xff);//Skip a stuff byte when stop reading
  //�ȴ���Ӧ����ʱ�˳�
  Retry=0X1F;
  do
  {
    r1=SD_SPI_Read_Write_Byte(0xFF);
  }while((r1&0X80) && Retry--);
  //����״ֵ̬
  return r1;
}


/*************************************************************************************
**��ȡSD����CID��Ϣ��������������Ϣ
**����: u8 *cid_data(���CID���ڴ棬����16Byte��
**����ֵ:0��NO_ERR
**		 1������
*************************************************************************************/
u8 SD_GetCID(u8 *cid_data)
{
  u8 r1;
  //��CMD10�����CID
  r1=SD_SendCmd(CMD10,0,0x01);
  if(r1==0x00)
  {
    r1=SD_RecvData(cid_data,16);//����16���ֽڵ�����
  }
  SD_Cancel_CS();//ȡ��Ƭѡ
  if(r1)  return 1;
  else    return 0;
}


/*************************************************************************************
��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
����:u8 *cid_data(���CID���ڴ棬����16Byte��
����ֵ:0��NO_ERR
		 1������
*************************************************************************************/
u8 SD_GetCSD(u8 *csd_data)
{
  u8 r1;
  r1=SD_SendCmd(CMD9,0,0x01);    //��CMD9�����CSD
  if(r1==0)
  {
    r1=SD_RecvData(csd_data, 16);//����16���ֽڵ�����
  }
  SD_Cancel_CS();//ȡ��Ƭѡ
  if(r1)  return 1;
  else    return 0;
}


/************************************************************
**��ȡSD����������������������
**����ֵ:0�� ȡ��������
**����:SD��������(������/512�ֽ�)
**ÿ�������ֽ�����Ϊ512�ֽڣ���Ϊ�������512�ֽڣ����ʼ������ͨ��.
*************************************************************/
u32 SD_Get_Sector_Count(void)
{
  u8 csd[16];
  u32 Capacity;
  u8 n;
  u16 csize;
  if(SD_GetCSD(csd) != 0) return 0;	//ȡCSD��Ϣ������ڼ����������0
  if((csd[0]&0xC0) == 0x40)	        //V2.00�Ŀ�,���ΪSDHC�����������淽ʽ����
  {
    csize = csd[9] + ((u16)csd[8] << 8) + 1;
    Capacity = (u32)csize << 10;//�õ�������
  }
  else//V1.XX�Ŀ�
  {
    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
    csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
    Capacity= (u32)csize << (n - 9);//�õ�������
  }
  return Capacity;
}



/**********************************
��ʼ��SD��
***********************************/

u8 SD_Init(void)
{
  u8 r1;      // ���SD���ķ���ֵ
  u16 retry;  // �������г�ʱ����
  u8 buf[4];
  u16 i;

  SD_SPI_Init();		//��ʼ��IO
  SD_SetLowSpeed();
  for(i=0;i<10;i++)SD_SPI_Read_Write_Byte(0XFF); //��������74������
  retry=20;
  do
  {
    r1 = SD_SendCmd(CMD0,0,0x95);//����IDLE״̬ ����
  }while((r1!=0X01) && retry--);
  SD_Type=0;   //Ĭ���޿�
  if(r1==0X01)
  {
    if(SD_SendCmd(CMD8,0x1AA,0x87)==1)  //SD V2.0
    {
      for(i=0;i<4;i++)buf[i]=SD_SPI_Read_Write_Byte(0XFF);	//Get trailing return value of R7 resp
      if(buf[2]==0X01&&buf[3]==0XAA)    //���Ƿ�֧��2.7~3.6V
      {
        retry = 0XFFFE;
        do
        {
          SD_SendCmd(CMD55,0,0X01);	    //����CMD55
          r1 = SD_SendCmd(CMD41,0x40000000,0X01);//����CMD41
        }while(r1 && retry--);
        if(retry&&SD_SendCmd(CMD58,0,0X01) == 0)//����SD2.0���汾��ʼ
        {
          for(i=0;i<4;i++)buf[i]=SD_SPI_Read_Write_Byte(0XFF);//�õ�OCRֵ
          if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC;    //���CCS
          else SD_Type=SD_TYPE_V2;
        }
      }
    }
    else//SD V1.x/ MMC	V3
    {
      SD_SendCmd(CMD55,0,0X01);		//����CMD55
      r1 = SD_SendCmd(CMD41,0,0X01);	//����CMD41
      if(r1 <= 1)
      {
        SD_Type = SD_TYPE_V1;
        retry = 0XFFFE;
        do //�ȴ��˳�IDLEģʽ
        {
          SD_SendCmd(CMD55,0,0X01);	//����CMD55
          r1 = SD_SendCmd(CMD41,0,0X01);//����CMD41
        }while(r1 && retry--);
      }
      else//MMC����֧��CMD55+CMD41ʶ��
      {
        SD_Type = SD_TYPE_MMC;//MMC V3
        retry = 0XFFFE;
        do //�ȴ��˳�IDLEģʽ
        {
          r1 = SD_SendCmd(CMD1,0,0X01);//����CMD1
        }while(r1 && retry--);
      }
      if(retry==0 || SD_SendCmd(CMD16,512,0X01)!=0)
        SD_Type = SD_TYPE_ERR;//����Ŀ�
    }
  }
  SD_Cancel_CS();    //ȡ��Ƭѡ
  SD_SetHighSpeed();
  if(SD_Type) return 0;
  else if(r1) return r1;
  return 0xaa;//��������
}


/*************************************************************************************
**��SD��
**buf:���ݻ�����
**sector:����
**cnt:������
**����ֵ:0,ok;����,ʧ��.
*************************************************************************************/
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt) 	//��SD��,fatfs/usb����x
{
  u8 r1;
  if(SD_Type != SD_TYPE_V2HC) sector <<= 9;//ת��Ϊ�ֽڵ�ַ
  if(cnt == 1)
  {
    r1 = SD_SendCmd(CMD17, sector, 0X01);//������
    if(r1 == 0)												//ָ��ͳɹ�
    {
      r1 = SD_RecvData(buf,512);			//����512���ֽ�
    }
  }else
  {
    r1 = SD_SendCmd(CMD18, sector, 0X01);//����������
    do
    {
      r1 = SD_RecvData(buf, 512);//����512���ֽ�
      buf += 512;
    }while(--cnt && r1==0);
    SD_SendCmd(CMD12, 0, 0X01);	//����ֹͣ����
  }
  SD_Cancel_CS();//ȡ��Ƭѡ
  return r1;//
}

/*************************************************************************************
**дSD��
**buf:���ݻ�����
**sector:��ʼ����
**cnt:������
**����ֵ:0,ok;����,ʧ��.
*************************************************************************************/
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt)	//дSD��,fatfs/usb����
{
  u8 r1;
  if(SD_Type!=SD_TYPE_V2HC) sector *= 512;//ת��Ϊ�ֽڵ�ַ
  if(cnt == 1)
  {
    r1 = SD_SendCmd(CMD24, sector, 0X01);//������
    if(r1 == 0)//ָ��ͳɹ�
    {
      r1 = SD_Send_Data(buf, 0xFE);//д512���ֽ�
    }
  }else
  {
    if(SD_Type != SD_TYPE_MMC)
    {
      SD_SendCmd(CMD55, 0, 0X01);
      SD_SendCmd(CMD23, cnt, 0X01);//����ָ��
    }
    r1 = SD_SendCmd(CMD25, sector, 0X01);//����������
    if(r1 == 0)
    {
      do
      {
        r1 = SD_Send_Data(buf, 0xFC);//����512���ֽ�
        buf += 512;
      }while(--cnt && r1==0);
      r1 = SD_Send_Data(0, 0xFD);//����512���ֽ�
    }
  }
  SD_Cancel_CS();//ȡ��Ƭѡ
  return r1;//
}

#endif
