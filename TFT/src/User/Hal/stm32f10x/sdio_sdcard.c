#include "variants.h"

#ifdef SD_SDIO_SUPPORT

#include "sdio_sdcard.h"
#include "string.h"
#include "GPIO_Init.h"

static u8 CardType=SDIO_STD_CAPACITY_SD_CARD_V1_1;
static u32 CSD_Tab[4],CID_Tab[4],RCA=0;
static u8 DeviceMode=SD_DMA_MODE;
static u8 StopCondition=0;
volatile SD_Error TransferError=SD_OK;
volatile u8 TransferEnd=0;
SD_CardInfo SDCardInfo;

u8 SDIO_DATA_BUFFER[512];


void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)
{
	u32 temp,temp1;
	temp1=(~NVIC_Group)&0x07;
	temp1<<=8;
	temp=SCB->AIRCR;
	temp&=0X0000F8FF;
	temp|=0X05FA0000;
	temp|=temp1;
	SCB->AIRCR=temp;
}
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)
{
	u32 temp;
	MY_NVIC_PriorityGroupConfig(NVIC_Group);
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;
	NVIC->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);
	NVIC->IP[NVIC_Channel]|=temp<<4;
}

#ifdef SD_CD_PIN
//SD_CD insert detect
void SD_CD_WP_Init(void)
{
  GPIO_InitSet(SD_CD_PIN, MGPIO_MODE_IPU, 0);
}
#endif

uint8_t SD_CD_Inserted(void)
{
#ifdef SD_CD_PIN
  return !GPIO_GetLevel(SD_CD_PIN);
#else
  return 0;
#endif
}


SD_Error SD_Init(void)
{
	u8 clkdiv=0;
	SD_Error errorstatus=SD_OK;

#ifdef SD_CD_PIN
  SD_CD_WP_Init();
#endif

	RCC->APB2ENR|=1<<4;
	RCC->APB2ENR|=1<<5;
  RCC->AHBENR|=1<<10;
 	RCC->AHBENR|=1<<1;

	GPIOC->CRH&=0XFFF00000;
	GPIOC->CRH|=0X000BBBBB;	//PC.8~12

	GPIOD->CRL&=0XFFFFF0FF;
	GPIOD->CRL|=0X00000B00;	//PD2 PD7
 	//SDIO
	SDIO->POWER=0x00000000;
	SDIO->CLKCR=0x00000000;
	SDIO->ARG=0x00000000;
	SDIO->CMD=0x00000000;
	SDIO->DTIMER=0x00000000;
	SDIO->DLEN=0x00000000;
	SDIO->DCTRL=0x00000000;
	SDIO->ICR=0x00C007FF;
	SDIO->MASK=0x00000000;
 	MY_NVIC_Init(0,0,SDIO_IRQn,2);
  errorstatus=SD_PowerON();
  if(errorstatus==SD_OK) errorstatus=SD_InitializeCards();
  if(errorstatus==SD_OK) errorstatus=SD_GetCardInfo(&SDCardInfo);
  if(errorstatus==SD_OK) errorstatus=SD_SelectDeselect((u32)(SDCardInfo.RCA<<16));
  if(errorstatus==SD_OK) errorstatus=SD_EnableWideBusOperation(1);
  if((errorstatus==SD_OK)||(SDIO_MULTIMEDIA_CARD==CardType))
	{
		if(SDCardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1||SDCardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)
		{
			clkdiv=SDIO_TRANSFER_CLK_DIV+6;	//V1.1/V2.0 72/12=6Mhz
		}else clkdiv=SDIO_TRANSFER_CLK_DIV;	//SDHC 72/6=12Mhz
		SDIO_Clock_Set(clkdiv);				//SDIO_CK=SDIOCLK/[clkdiv+2]; 48Mhz
		//errorstatus=SD_SetDeviceMode(SD_DMA_MODE);
		errorstatus=SD_SetDeviceMode(SD_POLLING_MODE);
 	}
	return errorstatus;
}

void SD_DeInit(void)
{

}

void SDIO_Clock_Set(u8 clkdiv)
{
  u32 tmpreg=SDIO->CLKCR;
  tmpreg&=0XFFFFFF00;
  tmpreg|=clkdiv;
  SDIO->CLKCR=tmpreg;
}

void SDIO_Send_Cmd(u8 cmdindex,u8 waitrsp,u32 arg)
{
  u32 tmpreg;
  SDIO->ARG=arg;
  tmpreg=SDIO->CMD;
  tmpreg&=0XFFFFF800;
  tmpreg|=cmdindex&0X3F;
  tmpreg|=waitrsp<<6;
  tmpreg|=0<<8;
  tmpreg|=1<<10;
  SDIO->CMD=tmpreg;
}

void SDIO_Send_Data_Cfg(u32 datatimeout,u32 datalen,u8 blksize,u8 dir)
{
  u32 tmpreg;
  SDIO->DTIMER=datatimeout;
  SDIO->DLEN=datalen&0X1FFFFFF;
  tmpreg=SDIO->DCTRL;
  tmpreg&=0xFFFFFF08;
  tmpreg|=blksize<<4;
  tmpreg|=0<<2;
  tmpreg|=(dir&0X01)<<1;
  tmpreg|=1<<0;
  SDIO->DCTRL=tmpreg;
}

SD_Error SD_PowerON(void)
{
  u8 i=0;
  SD_Error errorstatus=SD_OK;
  u32 response=0,count=0,validvoltage=0;
  u32 SDType=SD_STD_CAPACITY;

  SDIO->CLKCR=0;
  SDIO->CLKCR|=0<<9;
  SDIO->CLKCR|=0<<10;
  SDIO->CLKCR|=0<<13;
  SDIO->CLKCR|=0<<14;
  SDIO_Clock_Set(SDIO_INIT_CLK_DIV);
  SDIO->POWER=0X03;
  SDIO->CLKCR|=1<<8;
  for(i=0;i<74;i++)
  {
    SDIO_Send_Cmd(SD_CMD_GO_IDLE_STATE,0,0);
    errorstatus=CmdError();
    if(errorstatus==SD_OK)break;
  }
  if(errorstatus)return errorstatus;
  SDIO_Send_Cmd(SDIO_SEND_IF_COND,1,SD_CHECK_PATTERN);

  errorstatus=CmdResp7Error();
 	if(errorstatus==SD_OK)
	{
		CardType=SDIO_STD_CAPACITY_SD_CARD_V2_0;
		SDType=SD_HIGH_CAPACITY;
	}
	SDIO_Send_Cmd(SD_CMD_APP_CMD,1,0);
	errorstatus=CmdResp1Error(SD_CMD_APP_CMD);
	if(errorstatus==SD_OK)//SD2.0/SD 1.1
	{
		while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
		{
			SDIO_Send_Cmd(SD_CMD_APP_CMD,1,0);
			errorstatus=CmdResp1Error(SD_CMD_APP_CMD);
 			if(errorstatus!=SD_OK)return errorstatus;
			SDIO_Send_Cmd(SD_CMD_SD_APP_OP_COND,1,SD_VOLTAGE_WINDOW_SD|SDType);
			errorstatus=CmdResp3Error();
 			if(errorstatus!=SD_OK)return errorstatus;
			response=SDIO->RESP1;
			validvoltage=(((response>>31)==1)?1:0);
			count++;
		}
		if(count>=SD_MAX_VOLT_TRIAL)
		{
			errorstatus=SD_INVALID_VOLTRANGE;
			return errorstatus;
		}
		if(response&=SD_HIGH_CAPACITY)
		{
			CardType=SDIO_HIGH_CAPACITY_SD_CARD;
		}
 	}else//MMC��
	{
		//MMC��,����CMD1 SDIO_SEND_OP_COND,����Ϊ:0x80FF8000
		while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
		{
			SDIO_Send_Cmd(SD_CMD_SEND_OP_COND,1,SD_VOLTAGE_WINDOW_MMC);//����CMD1,����Ӧ
			errorstatus=CmdResp3Error(); 					//�ȴ�R3��Ӧ
 			if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
			response=SDIO->RESP1;;			   				//�õ���Ӧ
			validvoltage=(((response>>31)==1)?1:0);
			count++;
		}
		if(count>=SD_MAX_VOLT_TRIAL)
		{
			errorstatus=SD_INVALID_VOLTRANGE;
			return errorstatus;
		}
		CardType=SDIO_MULTIMEDIA_CARD;
  	}
  	return(errorstatus);
}
//SD�� Power OFF
//����ֵ:�������?;(0,�޴���)
SD_Error SD_PowerOFF(void)
{
  	SDIO->POWER&=~(3<<0);//SDIO��Դ�ر�,ʱ��ֹͣ
	return SD_OK;
}
//��ʼ�����еĿ�,���ÿ��������״�?
//����ֵ:�������?
SD_Error SD_InitializeCards(void)
{
 	SD_Error errorstatus=SD_OK;
	u16 rca = 0x01;
 	if((SDIO->POWER&0X03)==0)return SD_REQUEST_NOT_APPLICABLE;//����Դ״̬,ȷ��Ϊ�ϵ�״̬
 	if(SDIO_SECURE_DIGITAL_IO_CARD!=CardType)			//��SECURE_DIGITAL_IO_CARD
	{
		SDIO_Send_Cmd(SD_CMD_ALL_SEND_CID,3,0);			//����CMD2,ȡ��CID,����Ӧ
		errorstatus=CmdResp2Error(); 					//�ȴ�R2��Ӧ
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
 		CID_Tab[0]=SDIO->RESP1;
		CID_Tab[1]=SDIO->RESP2;
		CID_Tab[2]=SDIO->RESP3;
		CID_Tab[3]=SDIO->RESP4;
	}
	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_SECURE_DIGITAL_IO_COMBO_CARD==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))//�жϿ�����
	{
		SDIO_Send_Cmd(SD_CMD_SET_REL_ADDR,1,0);			//����CMD3,����Ӧ
		errorstatus=CmdResp6Error(SD_CMD_SET_REL_ADDR,&rca);//�ȴ�R6��Ӧ
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
	}
    if (SDIO_MULTIMEDIA_CARD==CardType)
    {
 		SDIO_Send_Cmd(SD_CMD_SET_REL_ADDR,1,(u32)(rca<<16));//����CMD3,����Ӧ
		errorstatus=CmdResp2Error(); 					//�ȴ�R2��Ӧ
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
    }
	if (SDIO_SECURE_DIGITAL_IO_CARD!=CardType)			//��SECURE_DIGITAL_IO_CARD
	{
		RCA = rca;
		SDIO_Send_Cmd(SD_CMD_SEND_CSD,3,(u32)(rca<<16));//����CMD9+��RCA,ȡ��CSD,����Ӧ
		errorstatus=CmdResp2Error(); 					//�ȴ�R2��Ӧ
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
  		CSD_Tab[0]=SDIO->RESP1;
		CSD_Tab[1]=SDIO->RESP2;
		CSD_Tab[2]=SDIO->RESP3;
		CSD_Tab[3]=SDIO->RESP4;
	}
	return SD_OK;//����ʼ���ɹ�
}
//�õ�����Ϣ
//cardinfo:����Ϣ�洢��
//����ֵ:����״̬
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{
 	SD_Error errorstatus=SD_OK;
	u8 tmp=0;
	cardinfo->CardType=(u8)CardType; 				//������
	cardinfo->RCA=(u16)RCA;							//��RCAֵ
	tmp=(u8)((CSD_Tab[0]&0xFF000000)>>24);
	cardinfo->SD_csd.CSDStruct=(tmp&0xC0)>>6;		//CSD�ṹ
	cardinfo->SD_csd.SysSpecVersion=(tmp&0x3C)>>2;	//2.0Э�黹û�����ⲿ��(Ϊ����),Ӧ���Ǻ���Э�鶨���?
	cardinfo->SD_csd.Reserved1=tmp&0x03;			//2������λ
	tmp=(u8)((CSD_Tab[0]&0x00FF0000)>>16);			//��1���ֽ�
	cardinfo->SD_csd.TAAC=tmp;				   		//���ݶ�ʱ��1
	tmp=(u8)((CSD_Tab[0]&0x0000FF00)>>8);	  		//��2���ֽ�
	cardinfo->SD_csd.NSAC=tmp;		  				//���ݶ�ʱ��2
	tmp=(u8)(CSD_Tab[0]&0x000000FF);				//��3���ֽ�
	cardinfo->SD_csd.MaxBusClkFrec=tmp;		  		//�����ٶ�
	tmp=(u8)((CSD_Tab[1]&0xFF000000)>>24);			//��4���ֽ�
	cardinfo->SD_csd.CardComdClasses=tmp<<4;    	//��ָ��������?
	tmp=(u8)((CSD_Tab[1]&0x00FF0000)>>16);	 		//��5���ֽ�
	cardinfo->SD_csd.CardComdClasses|=(tmp&0xF0)>>4;//��ָ��������?
	cardinfo->SD_csd.RdBlockLen=tmp&0x0F;	    	//����ȡ���ݳ���
	tmp=(u8)((CSD_Tab[1]&0x0000FF00)>>8);			//��6���ֽ�
	cardinfo->SD_csd.PartBlockRead=(tmp&0x80)>>7;	//�����ֿ��?
	cardinfo->SD_csd.WrBlockMisalign=(tmp&0x40)>>6;	//д����?
	cardinfo->SD_csd.RdBlockMisalign=(tmp&0x20)>>5;	//������?
	cardinfo->SD_csd.DSRImpl=(tmp&0x10)>>4;
	cardinfo->SD_csd.Reserved2=0; 					//����
 	if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1)||(CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)||(SDIO_MULTIMEDIA_CARD==CardType))//��׼1.1/2.0��/MMC��
	{
		cardinfo->SD_csd.DeviceSize=(tmp&0x03)<<10;	//C_SIZE(12λ)
	 	tmp=(u8)(CSD_Tab[1]&0x000000FF); 			//��7���ֽ�
		cardinfo->SD_csd.DeviceSize|=(tmp)<<2;
 		tmp=(u8)((CSD_Tab[2]&0xFF000000)>>24);		//��8���ֽ�
		cardinfo->SD_csd.DeviceSize|=(tmp&0xC0)>>6;
 		cardinfo->SD_csd.MaxRdCurrentVDDMin=(tmp&0x38)>>3;
		cardinfo->SD_csd.MaxRdCurrentVDDMax=(tmp&0x07);
 		tmp=(u8)((CSD_Tab[2]&0x00FF0000)>>16);		//��9���ֽ�
		cardinfo->SD_csd.MaxWrCurrentVDDMin=(tmp&0xE0)>>5;
		cardinfo->SD_csd.MaxWrCurrentVDDMax=(tmp&0x1C)>>2;
		cardinfo->SD_csd.DeviceSizeMul=(tmp&0x03)<<1;//C_SIZE_MULT
 		tmp=(u8)((CSD_Tab[2]&0x0000FF00)>>8);	  	//��10���ֽ�
		cardinfo->SD_csd.DeviceSizeMul|=(tmp&0x80)>>7;
 		cardinfo->CardCapacity=(cardinfo->SD_csd.DeviceSize+1);//���㿨����
		cardinfo->CardCapacity*=(1<<(cardinfo->SD_csd.DeviceSizeMul+2));
		cardinfo->CardBlockSize=1<<(cardinfo->SD_csd.RdBlockLen);//����?
		cardinfo->CardCapacity*=cardinfo->CardBlockSize;
	}else if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	//��������
	{
 		tmp=(u8)(CSD_Tab[1]&0x000000FF); 		//��7���ֽ�
		cardinfo->SD_csd.DeviceSize=(tmp&0x3F)<<16;//C_SIZE
 		tmp=(u8)((CSD_Tab[2]&0xFF000000)>>24); 	//��8���ֽ�
 		cardinfo->SD_csd.DeviceSize|=(tmp<<8);
 		tmp=(u8)((CSD_Tab[2]&0x00FF0000)>>16);	//��9���ֽ�
 		cardinfo->SD_csd.DeviceSize|=(tmp);
 		tmp=(u8)((CSD_Tab[2]&0x0000FF00)>>8); 	//��10���ֽ�
 		cardinfo->CardCapacity=(long long)(cardinfo->SD_csd.DeviceSize+1)*512*1024;//���㿨����
		cardinfo->CardBlockSize=512; 			//���С�̶��?512�ֽ�
	}
	cardinfo->SD_csd.EraseGrSize=(tmp&0x40)>>6;
	cardinfo->SD_csd.EraseGrMul=(tmp&0x3F)<<1;
	tmp=(u8)(CSD_Tab[2]&0x000000FF);			//��11���ֽ�
	cardinfo->SD_csd.EraseGrMul|=(tmp&0x80)>>7;
	cardinfo->SD_csd.WrProtectGrSize=(tmp&0x7F);
 	tmp=(u8)((CSD_Tab[3]&0xFF000000)>>24);		//��12���ֽ�
	cardinfo->SD_csd.WrProtectGrEnable=(tmp&0x80)>>7;
	cardinfo->SD_csd.ManDeflECC=(tmp&0x60)>>5;
	cardinfo->SD_csd.WrSpeedFact=(tmp&0x1C)>>2;
	cardinfo->SD_csd.MaxWrBlockLen=(tmp&0x03)<<2;
	tmp=(u8)((CSD_Tab[3]&0x00FF0000)>>16);		//��13���ֽ�
	cardinfo->SD_csd.MaxWrBlockLen|=(tmp&0xC0)>>6;
	cardinfo->SD_csd.WriteBlockPaPartial=(tmp&0x20)>>5;
	cardinfo->SD_csd.Reserved3=0;
	cardinfo->SD_csd.ContentProtectAppli=(tmp&0x01);
	tmp=(u8)((CSD_Tab[3]&0x0000FF00)>>8);		//��14���ֽ�
	cardinfo->SD_csd.FileFormatGrouop=(tmp&0x80)>>7;
	cardinfo->SD_csd.CopyFlag=(tmp&0x40)>>6;
	cardinfo->SD_csd.PermWrProtect=(tmp&0x20)>>5;
	cardinfo->SD_csd.TempWrProtect=(tmp&0x10)>>4;
	cardinfo->SD_csd.FileFormat=(tmp&0x0C)>>2;
	cardinfo->SD_csd.ECC=(tmp&0x03);
	tmp=(u8)(CSD_Tab[3]&0x000000FF);			//��15���ֽ�
	cardinfo->SD_csd.CSD_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_csd.Reserved4=1;
	tmp=(u8)((CID_Tab[0]&0xFF000000)>>24);		//��0���ֽ�
	cardinfo->SD_cid.ManufacturerID=tmp;
	tmp=(u8)((CID_Tab[0]&0x00FF0000)>>16);		//��1���ֽ�
	cardinfo->SD_cid.OEM_AppliID=tmp<<8;
	tmp=(u8)((CID_Tab[0]&0x000000FF00)>>8);		//��2���ֽ�
	cardinfo->SD_cid.OEM_AppliID|=tmp;
	tmp=(u8)(CID_Tab[0]&0x000000FF);			//��3���ֽ�
	cardinfo->SD_cid.ProdName1=tmp<<24;
	tmp=(u8)((CID_Tab[1]&0xFF000000)>>24); 		//��4���ֽ�
	cardinfo->SD_cid.ProdName1|=tmp<<16;
	tmp=(u8)((CID_Tab[1]&0x00FF0000)>>16);	   	//��5���ֽ�
	cardinfo->SD_cid.ProdName1|=tmp<<8;
	tmp=(u8)((CID_Tab[1]&0x0000FF00)>>8);		//��6���ֽ�
	cardinfo->SD_cid.ProdName1|=tmp;
	tmp=(u8)(CID_Tab[1]&0x000000FF);	  		//��7���ֽ�
	cardinfo->SD_cid.ProdName2=tmp;
	tmp=(u8)((CID_Tab[2]&0xFF000000)>>24); 		//��8���ֽ�
	cardinfo->SD_cid.ProdRev=tmp;
	tmp=(u8)((CID_Tab[2]&0x00FF0000)>>16);		//��9���ֽ�
	cardinfo->SD_cid.ProdSN=tmp<<24;
	tmp=(u8)((CID_Tab[2]&0x0000FF00)>>8); 		//��10���ֽ�
	cardinfo->SD_cid.ProdSN|=tmp<<16;
	tmp=(u8)(CID_Tab[2]&0x000000FF);   			//��11���ֽ�
	cardinfo->SD_cid.ProdSN|=tmp<<8;
	tmp=(u8)((CID_Tab[3]&0xFF000000)>>24); 		//��12���ֽ�
	cardinfo->SD_cid.ProdSN|=tmp;
	tmp=(u8)((CID_Tab[3]&0x00FF0000)>>16);	 	//��13���ֽ�
	cardinfo->SD_cid.Reserved1|=(tmp&0xF0)>>4;
	cardinfo->SD_cid.ManufactDate=(tmp&0x0F)<<8;
	tmp=(u8)((CID_Tab[3]&0x0000FF00)>>8);		//��14���ֽ�
	cardinfo->SD_cid.ManufactDate|=tmp;
	tmp=(u8)(CID_Tab[3]&0x000000FF);			//��15���ֽ�
	cardinfo->SD_cid.CID_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_cid.Reserved2=1;
	return errorstatus;
}
//����SDIO���߿���(MMC����֧��4bitģʽ)
//wmode:λ��ģʽ.0,1λ���ݿ���;1,4λ���ݿ���;2,8λ���ݿ���
//����ֵ:SD������״̬
SD_Error SD_EnableWideBusOperation(u32 wmode)
{
  SD_Error errorstatus=SD_OK;
 	if(SDIO_MULTIMEDIA_CARD==CardType)return SD_UNSUPPORTED_FEATURE;//MMC����֧��
 	else if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
	{
		if(wmode>=2)return SD_UNSUPPORTED_FEATURE;//��֧��8λģʽ
 		else
		{
			errorstatus=SDEnWideBus(wmode);
 			if(SD_OK==errorstatus)
			{
				SDIO->CLKCR&=~(3<<11);		//����?ǰ��λ������
				SDIO->CLKCR|=(u16)wmode<<11;//1λ/4λ���߿���
				SDIO->CLKCR|=0<<14;			//������Ӳ��������
			}
		}
	}
	return errorstatus;
}
//����SD������ģʽ
//Mode:
//����ֵ:����״̬
SD_Error SD_SetDeviceMode(u32 Mode)
{
	SD_Error errorstatus = SD_OK;
 	if((Mode==SD_DMA_MODE)||(Mode==SD_POLLING_MODE))DeviceMode=Mode;
	else errorstatus=SD_INVALID_PARAMETER;
	return errorstatus;
}
//ѡ��
//����CMD7,ѡ����Ե��?(rca)Ϊaddr�Ŀ�,ȡ��������.����?0,�򶼲�ѡ��.
//addr:����RCA��ַ
SD_Error SD_SelectDeselect(u32 addr)
{
 	SDIO_Send_Cmd(SD_CMD_SEL_DESEL_CARD,1,addr);	//����CMD7,ѡ��,����Ӧ
   	return CmdResp1Error(SD_CMD_SEL_DESEL_CARD);
}
//SD����ȡһ����
//buf:�����ݻ�����(����4�ֽڶ���!!)
//addr:��ȡ��ַ
//blksize:����?
SD_Error SD_ReadBlock(u8 *buf,long long addr,u16 blksize)
{
	SD_Error errorstatus=SD_OK;
	u8 power;
  u32 count=0,*tempbuff=(u32*)buf;//ת��Ϊu32ָ��
	u32 timeout=SDIO_DATATIMEOUT;
  if(NULL==buf) return SD_INVALID_PARAMETER;
  SDIO->DCTRL=0x0;	//���ݿ��ƼĴ�������(��DMA)
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
	{
		blksize=512;
		addr>>=9;
	}
  	SDIO_Send_Data_Cfg(SD_DATATIMEOUT,0,0,0);	//���DPSM״̬������
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);
		SDIO_Send_Cmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//����CMD16+�������ݳ���Ϊblksize,����Ӧ
		errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
	}else return SD_INVALID_PARAMETER;
  	SDIO_Send_Data_Cfg(SD_DATATIMEOUT,blksize,power,1);	//blksize,����������
   	SDIO_Send_Cmd(SD_CMD_READ_SINGLE_BLOCK,1,addr);		//����CMD17+��addr��ַ����ȡ����,����Ӧ
	errorstatus=CmdResp1Error(SD_CMD_READ_SINGLE_BLOCK);//�ȴ�R1��Ӧ
	if(errorstatus!=SD_OK)return errorstatus;   		//��Ӧ����
 	if(DeviceMode==SD_POLLING_MODE)						//��ѯģʽ,��ѯ����
	{
 		INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
		while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<10)|(1<<9))))//������/CRC/��ʱ/���?(��־)/��ʼλ����
		{
			if(SDIO->STA&(1<<15))						//����������,��ʾ���ٴ���8����
			{
				for(count=0;count<8;count++)			//ѭ����ȡ����
				{
					*(tempbuff+count)=SDIO->FIFO;
				}
				tempbuff+=8;
				timeout=0X7FFFFF; 	//���������ʱ��?
			}else 	//������ʱ
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		}
		if(SDIO->STA&(1<<3))		//���ݳ�ʱ����
		{
	 		SDIO->ICR|=1<<3; 		//������־
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO->STA&(1<<1))	//���ݿ�CRC����
		{
	 		SDIO->ICR|=1<<1; 		//������־
			return SD_DATA_CRC_FAIL;
		}else if(SDIO->STA&(1<<5)) 	//����fifo�������?
		{
	 		SDIO->ICR|=1<<5; 		//������־
			return SD_RX_OVERRUN;
		}else if(SDIO->STA&(1<<9)) 	//������ʼλ����
		{
	 		SDIO->ICR|=1<<9; 		//������־
			return SD_START_BIT_ERR;
		}
		while(SDIO->STA&(1<<21))	//FIFO����,�����ڿ�������
		{
			*tempbuff=SDIO->FIFO;	//ѭ����ȡ����
			tempbuff++;
		}
		INTX_ENABLE();//�������ж�
		SDIO->ICR=0X5FF;	 		//������б��
	}else if(DeviceMode==SD_DMA_MODE)
	{
  	    SD_DMA_Config((u32*)buf,blksize,0);
		TransferError=SD_OK;
		StopCondition=0;			//�����?,����Ҫ����ֹͣ����ָ��
		TransferEnd=0;				//�����������λ�����жϷ�����?1
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//������Ҫ���ж�
	 	SDIO->DCTRL|=1<<3;		 	//SDIO DMAʹ��
 		while(((DMA2->ISR&0X2000)==RESET)&&(TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;//�ȴ��������?
		if(timeout==0)return SD_DATA_TIMEOUT;//��ʱ
		if(TransferError!=SD_OK)errorstatus=TransferError;
    }
 	return errorstatus;
}

u32 *tempbuff;
SD_Error SD_ReadMultiBlocks(u8 *buf,long long addr,u16 blksize,u32 nblks)
{
  SD_Error errorstatus=SD_OK;
  u8 power;
  u32 count=0;
	u32 timeout=SDIO_DATATIMEOUT;
	tempbuff=(u32*)buf;//ת��Ϊu32ָ��

  SDIO->DCTRL=0x0;		//���ݿ��ƼĴ�������(��DMA)
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
	{
		blksize=512;
		addr>>=9;
	}
  SDIO_Send_Data_Cfg(SD_DATATIMEOUT,0,0,0);	//���DPSM״̬������
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);
		SDIO_Send_Cmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//����CMD16+�������ݳ���Ϊblksize,����Ӧ
		errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
	}else return SD_INVALID_PARAMETER;
	if(nblks>1)											//����
	{
    if(nblks*blksize>SD_MAX_DATA_LENGTH) return SD_INVALID_PARAMETER;//�ж��Ƿ񳬹������ճ���
		SDIO_Send_Data_Cfg(SD_DATATIMEOUT,nblks*blksize,power,1);//nblks*blksize,512����?,����������
    SDIO_Send_Cmd(SD_CMD_READ_MULT_BLOCK,1,addr);	//����CMD18+��addr��ַ����ȡ����,����Ӧ
		errorstatus=CmdResp1Error(SD_CMD_READ_MULT_BLOCK);//�ȴ�R1��Ӧ
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
 		if(DeviceMode==SD_POLLING_MODE)
		{
			INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
			while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<8)|(1<<9))))//������/CRC/��ʱ/���?(��־)/��ʼλ����
			{
				if(SDIO->STA&(1<<15))						//����������,��ʾ���ٴ���8����
				{
					for(count=0;count<8;count++)			//ѭ����ȡ����
					{
						*(tempbuff+count)=SDIO->FIFO;
					}
					tempbuff+=8;
					timeout=0X7FFFFF; 	//���������ʱ��?
				}else 	//������ʱ
				{
					if(timeout==0)return SD_DATA_TIMEOUT;
					timeout--;
				}
			}
			if(SDIO->STA&(1<<3))		//���ݳ�ʱ����
			{
		 		SDIO->ICR|=1<<3; 		//������־
				return SD_DATA_TIMEOUT;
		 	}else if(SDIO->STA&(1<<1))	//���ݿ�CRC����
			{
		 		SDIO->ICR|=1<<1; 		//������־
				return SD_DATA_CRC_FAIL;
			}else if(SDIO->STA&(1<<5)) 	//����fifo�������?
			{
		 		SDIO->ICR|=1<<5; 		//������־
				return SD_RX_OVERRUN;
			}else if(SDIO->STA&(1<<9)) 	//������ʼλ����
			{
		 		SDIO->ICR|=1<<9; 		//������־
				return SD_START_BIT_ERR;
			}
			while(SDIO->STA&(1<<21))	//FIFO����,�����ڿ�������
			{
				*tempbuff=SDIO->FIFO;	//ѭ����ȡ����
				tempbuff++;
			}
	 		if(SDIO->STA&(1<<8))		//���ս���
			{
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
					SDIO_Send_Cmd(SD_CMD_STOP_TRANSMISSION,1,0);		//����CMD12+��������
					errorstatus=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);//�ȴ�R1��Ӧ
					if(errorstatus!=SD_OK)return errorstatus;
				}
 			}
			INTX_ENABLE();//�������ж�
	 		SDIO->ICR=0X5FF;	 		//������б��
 		}else if(DeviceMode==SD_DMA_MODE)
		{
	 	    SD_DMA_Config((u32*)buf,nblks*blksize,0);
	   		TransferError=SD_OK;
			StopCondition=1;			//����,��Ҫ����ֹͣ����ָ��
			TransferEnd=0;				//�����������λ�����жϷ�����?1
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//������Ҫ���ж�
		 	SDIO->DCTRL|=1<<3;		 						//SDIO DMAʹ��
	 		while(((DMA2->ISR&0X2000)==RESET)&&timeout)timeout--;//�ȴ��������?
			if(timeout==0)return SD_DATA_TIMEOUT;//��ʱ
			while((TransferEnd==0)&&(TransferError==SD_OK));
			if(TransferError!=SD_OK)errorstatus=TransferError;
		}
  	}
	return errorstatus;
}
//SD��д1����
//buf:���ݻ�����
//addr:д��ַ
//blksize:����?
//����ֵ:����״̬
SD_Error SD_WriteBlock(u8 *buf,long long addr,  u16 blksize)
{
	SD_Error errorstatus = SD_OK;
	u8  power=0,cardstate=0;
	u32 timeout=0,bytestransferred=0;
	u32 cardstatus=0,count=0,restwords=0;
	u32	tlen=blksize;						//�ܳ���(�ֽ�)
	u32*tempbuff=(u32*)buf;
 	if(buf==NULL)return SD_INVALID_PARAMETER;//��������
  SDIO->DCTRL=0x0;							//���ݿ��ƼĴ�������(��DMA)
  SDIO_Send_Data_Cfg(SD_DATATIMEOUT,0,0,0);	//���DPSM״̬������
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	//��������
	{
		blksize=512;
		addr>>=9;
	}
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);
		SDIO_Send_Cmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//����CMD16+�������ݳ���Ϊblksize,����Ӧ
		errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
	}else return SD_INVALID_PARAMETER;
  SDIO_Send_Cmd(SD_CMD_SEND_STATUS,1,(u32)RCA<<16);	//����CMD13,��ѯ����״̬,����Ӧ
	errorstatus=CmdResp1Error(SD_CMD_SEND_STATUS);		//�ȴ�R1��Ӧ
	if(errorstatus!=SD_OK)return errorstatus;
	cardstatus=SDIO->RESP1;
	timeout=SD_DATATIMEOUT;
  while(((cardstatus&0x00000100)==0)&&(timeout>0)) 	//���READY_FOR_DATAλ�Ƿ���λ
	{
		timeout--;
    SDIO_Send_Cmd(SD_CMD_SEND_STATUS,1,(u32)RCA<<16);//����CMD13,��ѯ����״̬,����Ӧ
		errorstatus=CmdResp1Error(SD_CMD_SEND_STATUS);	//�ȴ�R1��Ӧ
		if(errorstatus!=SD_OK)return errorstatus;
		cardstatus=SDIO->RESP1;
	}
	if(timeout==0)return SD_ERROR;
  SDIO_Send_Cmd(SD_CMD_WRITE_SINGLE_BLOCK,1,addr);	//����CMD24,д����ָ��,����Ӧ
	errorstatus=CmdResp1Error(SD_CMD_WRITE_SINGLE_BLOCK);//�ȴ�R1��Ӧ
	if(errorstatus!=SD_OK)return errorstatus;
	StopCondition=0;									//����д,����Ҫ����ֹͣ����ָ��
 	SDIO_Send_Data_Cfg(SD_DATATIMEOUT,blksize,power,0);	//blksize, ����������
	timeout=SDIO_DATATIMEOUT;
	if (DeviceMode == SD_POLLING_MODE)
	{
		INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
		while(!(SDIO->STA&((1<<10)|(1<<4)|(1<<1)|(1<<3)|(1<<9))))//���ݿ鷢�ͳɹ�/����/CRC/��ʱ/��ʼλ����
		{
			if(SDIO->STA&(1<<14))							//���������?,��ʾ���ٴ���8����
			{
				if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//����32�ֽ���
				{
					restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);

					for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
					{
						SDIO->FIFO=*tempbuff;
					}
				}else
				{
					for(count=0;count<8;count++)
					{
						SDIO->FIFO=*(tempbuff+count);
					}
					tempbuff+=8;
					bytestransferred+=32;
				}
				timeout=0X3FFFFFFF;	//д�������ʱ��?
			}else
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		}
		if(SDIO->STA&(1<<3))		//���ݳ�ʱ����
		{
	 		SDIO->ICR|=1<<3; 		//������־
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO->STA&(1<<1))	//���ݿ�CRC����
		{
	 		SDIO->ICR|=1<<1; 		//������־
			return SD_DATA_CRC_FAIL;
		}else if(SDIO->STA&(1<<4)) 	//����fifo�������?
		{
	 		SDIO->ICR|=1<<4; 		//������־
			return SD_TX_UNDERRUN;
		}else if(SDIO->STA&(1<<9)) 	//������ʼλ����
		{
	 		SDIO->ICR|=1<<9; 		//������־
			return SD_START_BIT_ERR;
		}
		INTX_ENABLE();//�������ж�
		SDIO->ICR=0X5FF;	 		//������б��
	}else if(DeviceMode==SD_DMA_MODE)
	{
		SD_DMA_Config((u32*)buf,blksize,1);//SDIO DMA����
   		TransferError=SD_OK;
		StopCondition=0;			//����д,����Ҫ����ֹͣ����ָ��
		TransferEnd=0;				//�����������λ�����жϷ�����?1
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//���ò������ݽ�������ж�?
 	 	SDIO->DCTRL|=1<<3;								//SDIO DMAʹ��.
 		while(((DMA2->ISR&0X2000)==RESET)&&timeout)timeout--;//�ȴ��������?
		if(timeout==0)
		{
  			SD_Init();	 					//���³�ʼ��SD��,���Խ��д������������?
			return SD_DATA_TIMEOUT;			//��ʱ
 		}
		timeout=SDIO_DATATIMEOUT;
		while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
 		if(timeout==0)return SD_DATA_TIMEOUT;			//��ʱ
  		if(TransferError!=SD_OK)return TransferError;
 	}
 	SDIO->ICR=0X5FF;	 		//������б��
 	errorstatus=IsCardProgramming(&cardstate);
 	while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
	{
		errorstatus=IsCardProgramming(&cardstate);
	}
	return errorstatus;
}
//SD��д�����?
//buf:���ݻ�����
//addr:д��ַ
//blksize:����?
//nblks:Ҫд��Ŀ���?
//����ֵ:����״̬
SD_Error SD_WriteMultiBlocks(u8 *buf,long long addr,u16 blksize,u32 nblks)
{
	SD_Error errorstatus = SD_OK;
	u8  power = 0, cardstate = 0;
	u32 timeout=0,bytestransferred=0;
	u32 count = 0, restwords = 0;
	u32 tlen=nblks*blksize;				//�ܳ���(�ֽ�)
	u32 *tempbuff = (u32*)buf;
  if(buf==NULL)return SD_INVALID_PARAMETER; //��������
  SDIO->DCTRL=0x0;							//���ݿ��ƼĴ�������(��DMA)
  SDIO_Send_Data_Cfg(SD_DATATIMEOUT,0,0,0);	//���DPSM״̬������
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
	{
		blksize=512;
		addr>>=9;
	}
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);
		SDIO_Send_Cmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//����CMD16+�������ݳ���Ϊblksize,����Ӧ
		errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
	}else return SD_INVALID_PARAMETER;
	if(nblks>1)
	{
		if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;
     	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
    	{
			//�������?
	 	   	SDIO_Send_Cmd(SD_CMD_APP_CMD,1,(u32)RCA<<16);	//����ACMD55,����Ӧ
			errorstatus=CmdResp1Error(SD_CMD_APP_CMD);		//�ȴ�R1��Ӧ
			if(errorstatus!=SD_OK)return errorstatus;
	 	   	SDIO_Send_Cmd(SD_CMD_SET_BLOCK_COUNT,1,nblks);	//����CMD23,���ÿ�����,����Ӧ
			errorstatus=CmdResp1Error(SD_CMD_SET_BLOCK_COUNT);//�ȴ�R1��Ӧ
			if(errorstatus!=SD_OK)return errorstatus;
		}
		SDIO_Send_Cmd(SD_CMD_WRITE_MULT_BLOCK,1,addr);		//����CMD25,���дָ��?,����Ӧ
		errorstatus=CmdResp1Error(SD_CMD_WRITE_MULT_BLOCK);	//�ȴ�R1��Ӧ
		if(errorstatus!=SD_OK)return errorstatus;
 	 	SDIO_Send_Data_Cfg(SD_DATATIMEOUT,nblks*blksize,power,0);//blksize, ����������
		if(DeviceMode==SD_POLLING_MODE)
	    {
			timeout=SDIO_DATATIMEOUT;
			INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
			while(!(SDIO->STA&((1<<4)|(1<<1)|(1<<8)|(1<<3)|(1<<9))))//����/CRC/���ݽ���/��ʱ/��ʼλ����
			{
				if(SDIO->STA&(1<<14))							//���������?,��ʾ���ٴ���8��(32�ֽ�)
				{
					if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//����32�ֽ���
					{
						restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
						for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
						{
							SDIO->FIFO=*tempbuff;
						}
					}else 										//���������?,���Է�������8��(32�ֽ�)����
					{
						for(count=0;count<SD_HALFFIFO;count++)
						{
							SDIO->FIFO=*(tempbuff+count);
						}
						tempbuff+=SD_HALFFIFO;
						bytestransferred+=SD_HALFFIFOBYTES;
					}
					timeout=0X3FFFFFFF;	//д�������ʱ��?
				}else
				{
					if(timeout==0)return SD_DATA_TIMEOUT;
					timeout--;
				}
			}
			if(SDIO->STA&(1<<3))		//���ݳ�ʱ����
			{
		 		SDIO->ICR|=1<<3; 		//������־
				return SD_DATA_TIMEOUT;
		 	}else if(SDIO->STA&(1<<1))	//���ݿ�CRC����
			{
		 		SDIO->ICR|=1<<1; 		//������־
				return SD_DATA_CRC_FAIL;
			}else if(SDIO->STA&(1<<4)) 	//����fifo�������?
			{
		 		SDIO->ICR|=1<<4; 		//������־
				return SD_TX_UNDERRUN;
			}else if(SDIO->STA&(1<<9)) 	//������ʼλ����
			{
		 		SDIO->ICR|=1<<9; 		//������־
				return SD_START_BIT_ERR;
			}
			if(SDIO->STA&(1<<8))		//���ͽ���
			{
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
					SDIO_Send_Cmd(SD_CMD_STOP_TRANSMISSION,1,0);		//����CMD12+��������
					errorstatus=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);//�ȴ�R1��Ӧ
					if(errorstatus!=SD_OK)return errorstatus;
				}
			}
			INTX_ENABLE();//�������ж�
	 		SDIO->ICR=0X5FF;	 		//������б��
	    }else if(DeviceMode==SD_DMA_MODE)
		{
			SD_DMA_Config((u32*)buf,nblks*blksize,1);//SDIO DMA����
      TransferError=SD_OK;
			StopCondition=1;			//����?,��Ҫ����ֹͣ����ָ��
			TransferEnd=0;				//�����������λ�����жϷ�����?1
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//���ò������ݽ�������ж�?
	 	 	SDIO->DCTRL|=1<<3;								//SDIO DMAʹ��.
			timeout=SDIO_DATATIMEOUT;
	 		while(((DMA2->ISR&0X2000)==RESET)&&timeout)timeout--;//�ȴ��������?
			if(timeout==0)	 								//��ʱ
			{
  				SD_Init();	 					//���³�ʼ��SD��,���Խ��д������������?
	 			return SD_DATA_TIMEOUT;			//��ʱ
	 		}
			timeout=SDIO_DATATIMEOUT;
			while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
	 		if(timeout==0)return SD_DATA_TIMEOUT;			//��ʱ
	 		if(TransferError!=SD_OK)return TransferError;
		}
  	}
 	SDIO->ICR=0X5FF;	 		//������б��
 	errorstatus=IsCardProgramming(&cardstate);
 	while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
	{
		errorstatus=IsCardProgramming(&cardstate);
	}
	return errorstatus;
}
//SDIO�жϷ�����
void SDIO_IRQHandler(void)
{
 	SD_ProcessIRQSrc();//��������SDIO����ж�?
}
//SDIO�жϴ�������
//����SDIO��������еĸ����ж�����?
//����ֵ:�������?
SD_Error SD_ProcessIRQSrc(void)
{
	if(SDIO->STA&(1<<8))//��������ж�?
	{
		if (StopCondition==1)
		{
			SDIO_Send_Cmd(SD_CMD_STOP_TRANSMISSION,1,0);		//����CMD12,��������
			TransferError=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
		}else TransferError = SD_OK;
 		SDIO->ICR|=1<<8;//�������жϱ��?
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�?
 		TransferEnd = 1;
		return(TransferError);
	}
 	if(SDIO->STA&(1<<1))//����CRC����
	{
		SDIO->ICR|=1<<1;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�?
	    TransferError = SD_DATA_CRC_FAIL;
	    return(SD_DATA_CRC_FAIL);
	}
 	if(SDIO->STA&(1<<3))//���ݳ�ʱ����
	{
		SDIO->ICR|=1<<3;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�?
	    TransferError = SD_DATA_TIMEOUT;
	    return(SD_DATA_TIMEOUT);
	}
  	if(SDIO->STA&(1<<5))//FIFO�������?
	{
		SDIO->ICR|=1<<5;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�?
	    TransferError = SD_RX_OVERRUN;
	    return(SD_RX_OVERRUN);
	}
   	if(SDIO->STA&(1<<4))//FIFO�������?
	{
		SDIO->ICR|=1<<4;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�?
	    TransferError = SD_TX_UNDERRUN;
	    return(SD_TX_UNDERRUN);
	}
	if(SDIO->STA&(1<<9))//��ʼλ����
	{
		SDIO->ICR|=1<<9;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�?
	    TransferError = SD_START_BIT_ERR;
	    return(SD_START_BIT_ERR);
	}
	return(SD_OK);
}

//���CMD0��ִ��״̬
//����ֵ:sd��������
SD_Error CmdError(void)
{
	SD_Error errorstatus = SD_OK;
	u32 timeout=SDIO_CMD0TIMEOUT;
	while(timeout--)
	{
		if(SDIO->STA&(1<<7))break;	//�����ѷ���(������Ӧ)
	}
	if(timeout==0)return SD_CMD_RSP_TIMEOUT;
	SDIO->ICR=0X5FF;				//������
	return errorstatus;
}
//���R7��Ӧ�Ĵ���״̬
//����ֵ:sd��������
SD_Error CmdResp7Error(void)
{
	SD_Error errorstatus=SD_OK;
	u32 status = 0;
	u32 timeout=SDIO_CMD0TIMEOUT;
 	while(timeout--)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�?)
	}
 	if((timeout==0)||(status&(1<<2)))	//��Ӧ��ʱ
	{
		errorstatus=SD_CMD_RSP_TIMEOUT;	//��ǰ������2.0���ݿ�,���߲�֧���趨�ĵ�ѹ��Χ
		SDIO->ICR|=1<<2;				//���������Ӧ��ʱ���?
		return errorstatus;
	}
	if(status&1<<6)						//�ɹ����յ���Ӧ
	{
		errorstatus=SD_OK;
		SDIO->ICR|=1<<6;				//�����Ӧ���?
 	}
	return errorstatus;
}
//���R1��Ӧ�Ĵ���״̬
//cmd:��ǰ����
//����ֵ:sd��������
SD_Error CmdResp1Error(u8 cmd)
{
   	u32 status;
	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�?)
	}
	if(status&(1<<2))					//��Ӧ��ʱ
	{
 		SDIO->ICR=1<<2;					//���������Ӧ��ʱ���?
		return SD_CMD_RSP_TIMEOUT;
	}
 	if(status&(1<<0))					//CRC����
	{
 		SDIO->ICR=1<<0;					//������?
		return SD_CMD_CRC_FAIL;
	}
	if(SDIO->RESPCMD!=cmd)return SD_ILLEGAL_CMD;//���ƥ��
  	SDIO->ICR=0X5FF;	 				//������
	return (SD_Error)(SDIO->RESP1&SD_OCR_ERRORBITS);//���ؿ���Ӧ
}
//���R3��Ӧ�Ĵ���״̬
//����ֵ:����״̬
SD_Error CmdResp3Error(void)
{
	u32 status;
 	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�?)
	}
 	if(status&(1<<2))					//��Ӧ��ʱ
	{
		SDIO->ICR|=1<<2;				//���������Ӧ��ʱ���?
		return SD_CMD_RSP_TIMEOUT;
	}
   	SDIO->ICR=0X5FF;	 				//������
 	return SD_OK;
}
//���R2��Ӧ�Ĵ���״̬
//����ֵ:����״̬
SD_Error CmdResp2Error(void)
{
	SD_Error errorstatus=SD_OK;
	u32 status = 0;
	u32 timeout=SDIO_CMD0TIMEOUT;
 	while(timeout--)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�?)
	}
  	if((timeout==0)||(status&(1<<2)))	//��Ӧ��ʱ
	{
		errorstatus=SD_CMD_RSP_TIMEOUT;
		SDIO->ICR|=1<<2;				//���������Ӧ��ʱ���?
		return errorstatus;
	}
	if(status&1<<0)						//CRC����
	{
		errorstatus=SD_CMD_CRC_FAIL;
		SDIO->ICR|=1<<0;				//�����Ӧ���?
 	}
	SDIO->ICR=0X5FF;	 				//������
 	return errorstatus;
}
//���R6��Ӧ�Ĵ���״̬
//cmd:֮ǰ���͵�����
//prca:�����ص�RCA��ַ
//����ֵ:����״̬
SD_Error CmdResp6Error(u8 cmd,u16*prca)
{
	SD_Error errorstatus=SD_OK;
	u32 status;
	u32 rspr1;
 	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�?)
	}
	if(status&(1<<2))					//��Ӧ��ʱ
	{
 		SDIO->ICR|=1<<2;				//���������Ӧ��ʱ���?
		return SD_CMD_RSP_TIMEOUT;
	}
	if(status&1<<0)						//CRC����
	{
		SDIO->ICR|=1<<0;				//�����Ӧ���?
 		return SD_CMD_CRC_FAIL;
	}
	if(SDIO->RESPCMD!=cmd)				//�ж��Ƿ���Ӧcmd����
	{
 		return SD_ILLEGAL_CMD;
	}
	SDIO->ICR=0X5FF;	 				//������б��
	rspr1=SDIO->RESP1;					//�õ���Ӧ
	if(SD_ALLZERO==(rspr1&(SD_R6_GENERAL_UNKNOWN_ERROR|SD_R6_ILLEGAL_CMD|SD_R6_COM_CRC_FAILED)))
	{
		*prca=(u16)(rspr1>>16);			//����16λ�õ�,rca
		return errorstatus;
	}
   	if(rspr1&SD_R6_GENERAL_UNKNOWN_ERROR)return SD_GENERAL_UNKNOWN_ERROR;
   	if(rspr1&SD_R6_ILLEGAL_CMD)return SD_ILLEGAL_CMD;
   	if(rspr1&SD_R6_COM_CRC_FAILED)return SD_COM_CRC_FAILED;
	return errorstatus;
}

//SDIOʹ�ܿ�����ģʽ
//enx:0,��ʹ��;1,ʹ��;
//����ֵ:����״̬
SD_Error SDEnWideBus(u8 enx)
{
	SD_Error errorstatus = SD_OK;
 	u32 scr[2]={0,0};
	u8 arg=0X00;
	if(enx)arg=0X02;
	else arg=0X00;
 	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//SD������LOCKED״̬
 	errorstatus=FindSCR(RCA,scr);						//�õ�SCR�Ĵ�������
 	if(errorstatus!=SD_OK)return errorstatus;
	if((scr[1]&SD_WIDE_BUS_SUPPORT)!=SD_ALLZERO)		//֧�ֿ�����
	{
	 	SDIO_Send_Cmd(SD_CMD_APP_CMD,1,(u32)RCA<<16);	//����CMD55+RCA,����Ӧ
	 	errorstatus=CmdResp1Error(SD_CMD_APP_CMD);
	 	if(errorstatus!=SD_OK)return errorstatus;
	 	SDIO_Send_Cmd(SD_CMD_APP_SD_SET_BUSWIDTH,1,arg);//����ACMD6,����Ӧ,����:10,4λ;00,1λ.
		errorstatus=CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);
		return errorstatus;
	}else return SD_REQUEST_NOT_APPLICABLE;				//��֧�ֿ���������
}
//���?�Ƿ�����ִ��д����
//pstatus:��ǰ״̬.
//����ֵ:�������?
SD_Error IsCardProgramming(u8 *pstatus)
{
 	vu32 respR1 = 0, status = 0;
  	SDIO_Send_Cmd(SD_CMD_SEND_STATUS,1,(u32)RCA<<16);		//����CMD13
  	status=SDIO->STA;
	while(!(status&((1<<0)|(1<<6)|(1<<2))))status=SDIO->STA;//�ȴ��������?
   	if(status&(1<<0))			//CRC���ʧ��?
	{
		SDIO->ICR|=1<<0;		//���������?
		return SD_CMD_CRC_FAIL;
	}
   	if(status&(1<<2))			//���ʱ
	{
		SDIO->ICR|=1<<2;		//���������?
		return SD_CMD_RSP_TIMEOUT;
	}
 	if(SDIO->RESPCMD!=SD_CMD_SEND_STATUS)return SD_ILLEGAL_CMD;
	SDIO->ICR=0X5FF;	 		//������б��
	respR1=SDIO->RESP1;
	*pstatus=(u8)((respR1>>9)&0x0000000F);
	return SD_OK;
}
//��ȡ��ǰ��״̬
//pcardstatus:��״̬
//����ֵ:�������?
SD_Error SD_SendStatus(uint32_t *pcardstatus)
{
	SD_Error errorstatus = SD_OK;
	if(pcardstatus==NULL)
	{
		errorstatus=SD_INVALID_PARAMETER;
		return errorstatus;
	}
 	SDIO_Send_Cmd(SD_CMD_SEND_STATUS,1,RCA<<16);	//����CMD13,����Ӧ
	errorstatus=CmdResp1Error(SD_CMD_SEND_STATUS);	//��ѯ��Ӧ״̬
	if(errorstatus!=SD_OK)return errorstatus;
	*pcardstatus=SDIO->RESP1;//��ȡ��Ӧֵ
	return errorstatus;
}
//����SD����״̬
//����ֵ:SD��״̬
SDCardState SD_GetState(void)
{
	u32 resp1=0;
	if(SD_SendStatus(&resp1)!=SD_OK)return SD_CARD_ERROR;
	else return (SDCardState)((resp1>>9) & 0x0F);
}
//����SD����SCR�Ĵ���ֵ
//rca:����Ե��?
//pscr:���ݻ�����(�洢SCR����)
//����ֵ:����״̬
SD_Error FindSCR(u16 rca,u32 *pscr)
{
	u32 index = 0;
	SD_Error errorstatus = SD_OK;
	u32 tempscr[2]={0,0};
 	SDIO_Send_Cmd(SD_CMD_SET_BLOCKLEN,1,8);			//����CMD16,����Ӧ,����Block SizeΪ8�ֽ�
 	errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);
 	if(errorstatus!=SD_OK)return errorstatus;
  	SDIO_Send_Cmd(SD_CMD_APP_CMD,1,(u32)rca<<16);	//����CMD55,����Ӧ
 	errorstatus=CmdResp1Error(SD_CMD_APP_CMD);
 	if(errorstatus!=SD_OK)return errorstatus;
	SDIO_Send_Data_Cfg(SD_DATATIMEOUT,8,3,1);		//8���ֽڳ���,blockΪ8�ֽ�,SD����SDIO.
   	SDIO_Send_Cmd(SD_CMD_SD_APP_SEND_SCR,1,0);		//����ACMD51,����Ӧ,����Ϊ0
 	errorstatus=CmdResp1Error(SD_CMD_SD_APP_SEND_SCR);
 	if(errorstatus!=SD_OK)return errorstatus;
 	while(!(SDIO->STA&(SDIO_FLAG_RXOVERR|SDIO_FLAG_DCRCFAIL|SDIO_FLAG_DTIMEOUT|SDIO_FLAG_DBCKEND|SDIO_FLAG_STBITERR)))
	{
		if(SDIO->STA&(1<<21))//����FIFO���ݿ���
		{
			*(tempscr+index)=SDIO->FIFO;	//��ȡFIFO����
			index++;
			if(index>=2)break;
		}
	}
 	if(SDIO->STA&(1<<3))		//�������ݳ�ʱ
	{
 		SDIO->ICR|=1<<3;		//������
		return SD_DATA_TIMEOUT;
	}
	else if(SDIO->STA&(1<<1))	//�ѷ���/���յ����ݿ�CRCУ�����?
	{
 		SDIO->ICR|=1<<1;		//������
		return SD_DATA_CRC_FAIL;
	}
	else if(SDIO->STA&(1<<5))	//����FIFO���?
	{
 		SDIO->ICR|=1<<5;		//������
		return SD_RX_OVERRUN;
	}
	else if(SDIO->STA&(1<<9))	//��ʼλ������
	{
 		SDIO->ICR|=1<<9;		//������
		return SD_START_BIT_ERR;
	}
   	SDIO->ICR=0X5FF;	 		//������
	//������˳��8λΪ��λ������.
	*(pscr+1)=((tempscr[0]&SD_0TO7BITS)<<24)|((tempscr[0]&SD_8TO15BITS)<<8)|((tempscr[0]&SD_16TO23BITS)>>8)|((tempscr[0]&SD_24TO31BITS)>>24);
	*(pscr)=((tempscr[1]&SD_0TO7BITS)<<24)|((tempscr[1]&SD_8TO15BITS)<<8)|((tempscr[1]&SD_16TO23BITS)>>8)|((tempscr[1]&SD_24TO31BITS)>>24);
 	return errorstatus;
}
//�õ�NumberOfBytes��2Ϊ�׵�ָ��.
//NumberOfBytes:�ֽ���.
//����ֵ:��2Ϊ�׵�ָ��ֵ
u8 convert_from_bytes_to_power_of_two(u16 NumberOfBytes)
{
	u8 count=0;
	while(NumberOfBytes!=1)
	{
		NumberOfBytes>>=1;
		count++;
	}
	return count;
}
//����SDIO DMA
//mbuf:�洢����ַ
//bufsize:����������
//dir:����;1,�洢��-->SDIO(д����);0,SDIO-->�洢��(������);
void SD_DMA_Config(u32*mbuf,u32 bufsize,u8 dir)
{
 	DMA2->IFCR|=(0XF<<12);				//���DMA2ͨ��4�ĸ��ֱ��?
 	DMA2_Channel4->CCR&=~(1<<0);		//�ر�DMA ͨ��4
  DMA2_Channel4->CCR&=~(0X7FF<<4);	//����?ǰ������,DIR,CIRC,PINC,MINC,PSIZE,MSIZE,PL,MEM2MEM
 	DMA2_Channel4->CCR|=dir<<4;  		//�Ӵ洢����
	DMA2_Channel4->CCR|=0<<5;  			//��ͨģʽ
	DMA2_Channel4->CCR|=0<<6; 			//�����ַ������ģ�?
	DMA2_Channel4->CCR|=1<<7;  			//�洢������ģʽ
	DMA2_Channel4->CCR|=2<<8;  			//�������ݿ���Ϊ32λ
	DMA2_Channel4->CCR|=2<<10; 			//�洢�����ݿ���32λ
	DMA2_Channel4->CCR|=2<<12; 			//�����ȼ�
  DMA2_Channel4->CNDTR=bufsize/4;   	//DMA2,����������
 	DMA2_Channel4->CPAR=(u32)&SDIO->FIFO;//DMA2 ������?
	DMA2_Channel4->CMAR=(u32)mbuf; 		//DMA2,�洢����ַ
 	DMA2_Channel4->CCR|=1<<0; 			//����DMAͨ��
}
//��SD��
//buf:�����ݻ�����
//sector:������ַ
//cnt:��������
//����ֵ:����״̬;0,����;����,�������?;
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 sta=SD_OK;
	long long lsector=sector;
	u8 n;
	lsector<<=9;
	if((u32)buf%4!=0)
	{
	 	for(n=0;n<cnt;n++)
		{
		 	sta=SD_ReadBlock(SDIO_DATA_BUFFER,lsector+512*n,512);//����sector�Ķ�����
			memcpy(buf,SDIO_DATA_BUFFER,512);
			buf+=512;
		}
	}else
	{
		if(cnt==1)sta=SD_ReadBlock(buf,lsector,512);    	//����sector�Ķ�����
		else sta=SD_ReadMultiBlocks(buf,lsector,512,cnt);//���sector
	}
	return sta;
}
//дSD��
//buf:д���ݻ�����
//sector:������ַ
//cnt:��������
//����ֵ:����״̬;0,����;����,�������?;
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 sta=SD_OK;
	u8 n;
	long long lsector=sector;
	lsector<<=9;
	if((u32)buf%4!=0)
	{
	 	for(n=0;n<cnt;n++)
		{
			memcpy(SDIO_DATA_BUFFER,buf,512);
		 	sta=SD_WriteBlock(SDIO_DATA_BUFFER,lsector+512*n,512);//����sector��д����
			buf+=512;
		}
	}else
	{
		if(cnt==1)sta=SD_WriteBlock(buf,lsector,512);    	//����sector��д����
		else sta=SD_WriteMultiBlocks(buf,lsector,512,cnt);	//���sector
	}
	return sta;
}

#endif
