#ifndef _TOUCH_PROCESS_H_
#define _TOUCH_PROCESS_H_

#include "includes.h"


#define CMD_RDX 0xD0
#define CMD_RDY 0x90

enum
{
  KNOB_INC=1,
  KNOB_DEC,
};

#define KEY_CLICK          0x0000  // key����Ϊ u16, 16(u16 16bit) - 3(3 bits flag ) = 13 bit ���� u16 ���֧�� 2^13 = 8192 ����ֵ
#define KEY_DOUBLE_CLICK   0x2000  //����λ������ʶ˫������
#define KEY_LONG_RELEASE   0x4000  //�ڶ�λ������ʶ�������ͷŶ���
#define KEY_LONG_CLICK     0x8000  //��һλ������ʶ��������

extern bool touchSound;
void TSC_Calibration(void);
u8   isPress(void);
u16  KEY_GetValue(u8 total_rect,const GUI_RECT* menuRect);
u16  KNOB_GetRV(GUI_RECT *knob);

void loopTouchScreen(void);

extern void (*TSC_ReDrawIcon)(u8 positon, u8 is_press);
extern void TS_Get_Coordinates(u16 *x, u16 *y);

#endif
