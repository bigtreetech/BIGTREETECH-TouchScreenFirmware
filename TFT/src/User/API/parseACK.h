#ifndef _PARSE_ACK_H_
#define _PARSE_ACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "SerialConnection.h"

void setHostDialog(bool isHostDialog);
bool getHostDialog(void);
void setCurrentAckSrc(SERIAL_PORT_INDEX portIndex);
void parseACK(void);

#ifdef __cplusplus
}
#endif

#endif
