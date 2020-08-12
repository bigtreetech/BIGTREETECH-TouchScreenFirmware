#ifndef _SETTINGSMENU_H_
#define _SETTINGSMENU_H_

#include "stdint.h"
#include "Settings.h"

extern const u32 item_baudrate[ITEM_BAUDRATE_NUM];
extern uint8_t machine_type[64];

void menuInfo(void);
void menuSettings(void);
void infoSetFirmwareName(uint8_t *name, uint8_t name_len);
void infoSetMachineType(uint8_t *machine, uint8_t type_len);

#endif
