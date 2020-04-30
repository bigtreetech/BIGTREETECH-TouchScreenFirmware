#ifndef _PARSEACK_H_
#define _PARSEACK_H_

#include "stdint.h"
#include "Configuration.h"

static const char errormagic[]        = "Error:";
static const char echomagic[]         = "echo:";
static const char unknowmagic[]       = "Unknown command:";
static const char bsdprintingmagic[]   = "SD printing byte";
static const char bsdnoprintingmagic[] = "Not SD printing";


#define ACK_MAX_SIZE 2048
extern int MODEselect;

void setCurrentAckSrc(uint8_t src);
void parseACK(void);
void parseRcvGcode(void);

#endif
