#ifndef _SD_H_
#define _SD_H_

#include "spi.h"

// SD卡类型定义
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06

// SD卡指令表
#define CMD0    0       //卡复位
#define CMD1    1
#define CMD8    8       //命令8 ，SEND_IF_COND
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置扇区大小 应返回0x00
#define CMD17   17      //命令17，读扇区
#define CMD18   18      //命令18，读Multi 扇区
#define CMD23   23      //命令23，设置多扇区写入前预先擦除N个block
#define CMD24   24      //命令24，写扇区
#define CMD25   25      //命令25，写多个扇区
#define CMD41   41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00、

/*数据写入回应字意义*/
#define SD_DATA_OK                0x05
#define SD_DATA_CRC_ERROR         0x0B
#define SD_DATA_WRITE_ERROR       0x0D
#define SD_DATA_OTHER_ERROR       0xFF

/*SD卡回应标记字*/
#define SD_RESPONSE_NO_ERROR      0x00   //响应错误
#define SD_IN_IDLE_STATE          0x01   //闲置状态
#define SD_ERASE_RESET            0x02   //擦除复位
#define SD_ILLEGAL_COMMAND        0x04
#define SD_COM_CRC_ERROR          0x08
#define SD_ERASE_SEQUENCE_ERROR   0x10
#define SD_ADDRESS_ERROR          0x20
#define SD_PARAMETER_ERROR        0x40
#define SD_RESPONSE_FAILURE       0xFF

u8 SD_CD_Inserted(void);
u8 SD_Init(void);							        //初始化
void SD_DeInit(void);
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt); 	//读SD卡,fatfs/usb调用
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt);	//写SD卡,fatfs/usb调用

u8 SD_Wait_Ready(void);							    //等待SD卡准备
u8 SD_Get_Ack(u8 Response);					       	//获得应答
u32 SD_Get_Sector_Count(void);   					//读扇区数
u8 SD_GetCID(u8 *cid_data);                         //读SD卡CID
u8 SD_GetCSD(u8 *csd_data);                         //读SD卡CSD

#endif
