#ifndef _SD_H_
#define _SD_H_

#include "spi.h"

// SD�����Ͷ���
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06

// SD��ָ���
#define CMD0    0       //����λ
#define CMD1    1
#define CMD8    8       //����8 ��SEND_IF_COND
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������������С Ӧ����0x00
#define CMD17   17      //����17��������
#define CMD18   18      //����18����Multi ����
#define CMD23   23      //����23�����ö�����д��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��д����
#define CMD25   25      //����25��д�������
#define CMD41   41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00��

/*����д���Ӧ������*/
#define SD_DATA_OK                0x05
#define SD_DATA_CRC_ERROR         0x0B
#define SD_DATA_WRITE_ERROR       0x0D
#define SD_DATA_OTHER_ERROR       0xFF

/*SD����Ӧ�����*/
#define SD_RESPONSE_NO_ERROR      0x00   //��Ӧ����
#define SD_IN_IDLE_STATE          0x01   //����״̬
#define SD_ERASE_RESET            0x02   //������λ
#define SD_ILLEGAL_COMMAND        0x04
#define SD_COM_CRC_ERROR          0x08
#define SD_ERASE_SEQUENCE_ERROR   0x10
#define SD_ADDRESS_ERROR          0x20
#define SD_PARAMETER_ERROR        0x40
#define SD_RESPONSE_FAILURE       0xFF

u8 SD_CD_Inserted(void);
u8 SD_Init(void);							        //��ʼ��
void SD_DeInit(void);
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt); 	//��SD��,fatfs/usb����
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt);	//дSD��,fatfs/usb����

u8 SD_Wait_Ready(void);							    //�ȴ�SD��׼��
u8 SD_Get_Ack(u8 Response);					       	//���Ӧ��
u32 SD_Get_Sector_Count(void);   					//��������
u8 SD_GetCID(u8 *cid_data);                         //��SD��CID
u8 SD_GetCSD(u8 *csd_data);                         //��SD��CSD

#endif
