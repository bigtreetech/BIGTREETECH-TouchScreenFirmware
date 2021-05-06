#ifndef _SETTINGS_MENU_H_
#define _SETTINGS_MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern uint8_t machine_type[64];

void menuInfo(void);
void menuSettings(void);
void infoSetFirmwareName(uint8_t *name, uint8_t name_len);
void infoSetMachineType(uint8_t *machine, uint8_t type_len);
void infoSetAccessPoint(uint8_t *ssid, uint8_t ssid_len);
void infoSetIPAddress(uint8_t *ip, uint8_t ip_len);

#ifdef __cplusplus
}
#endif

#endif
