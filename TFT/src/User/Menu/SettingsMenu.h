#ifndef _SETTINGS_MENU_H_
#define _SETTINGS_MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern char machine_type[64];

void infoSetFirmwareName(char * name, uint8_t name_len);
void infoSetMachineType(char * machine, uint8_t type_len);
void infoSetAccessPoint(char * ssid, uint8_t ssid_len);
void infoSetIPAddress(char * ip, uint8_t ip_len);
void menuInfo(void);
void menuSettings(void);

#ifdef __cplusplus
}
#endif

#endif
