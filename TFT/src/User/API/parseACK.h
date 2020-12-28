#ifndef _PARSEACK_H_
#define _PARSEACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "Configuration.h"

static const char errormagic[]         = "Error:";
static const char echomagic[]          = "echo:";

#define ACK_MAX_SIZE 512

typedef enum                      // popup message types available to display an echo message
{
  ECHO_NOTIFY_NONE = 0,            // ignore the echo message
  ECHO_NOTIFY_TOAST,               // Show a non invasive toast on the title bar for a preset duration.
  ECHO_NOTIFY_DIALOG,              // Show a window to notify the user and alow interaction.
} ECHO_NOTIFY_TYPE;

typedef enum                           // append at the end of this list the id of any new echo message for
{                                      // which a specific popup message type must be used to notify the user
  ECHO_ID_BUSY = 0,
  ECHO_ID_FRESH_FILE,
  ECHO_ID_DOING_FILE,
  ECHO_ID_PROBE_OFFSET,
  ECHO_ID_FLOW,
  ECHO_ID_ECHO,
  ECHO_ID_ECHO_G,
  ECHO_ID_ECHO_M,
  ECHO_ID_CAP,
  ECHO_ID_CONFIG,
  ECHO_ID_SETTINGS,
  ECHO_ID_SOFT_ENDSTOP,
  ECHO_ID_BED_LEVELING,
  ECHO_ID_FADE_HEIGHT,
  ECHO_ID_TOOL_CHANGE,
  ECHO_ID_COUNT,
} ECHO_ID;

typedef struct
{
  ECHO_NOTIFY_TYPE  notifyType;
  const char *const msg;
} ECHO;

void setCurrentAckSrc(uint8_t src);
void parseACK(void);
void parseRcvGcode(void);

void setIgnoreEcho(ECHO_ID msgId, bool state);

#ifdef __cplusplus
}
#endif


#endif
