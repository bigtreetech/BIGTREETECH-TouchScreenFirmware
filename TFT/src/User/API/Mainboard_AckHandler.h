#ifndef _MAINBOARD_ACK_HANDLER_H_
#define _MAINBOARD_ACK_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "SerialConnection.h"

void setHostDialog(bool isHostDialog);
bool getHostDialog(void);
void setCurrentAckSrc(SERIAL_PORT_INDEX portIndex);
void parseAck(void);

#ifdef __cplusplus
}
#endif

#endif
