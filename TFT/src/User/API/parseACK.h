#ifndef _PARSE_ACK_H_
#define _PARSE_ACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define ACK_MAX_SIZE 512

extern bool hostDialog;

typedef enum  // append at the end of this list the id of any new echo message for
{             // which a specific popup message type must be used to notify the user
  ECHO_ID_BUSY_PAUSE = 0,
  ECHO_ID_BUSY_PROCESSING,
  ECHO_ID_FRESH_FILE,
  ECHO_ID_DOING_FILE,
  //ECHO_ID_PROBE_OFFSET,
  //ECHO_ID_ENQUEUE_M117,
  ECHO_ID_FLOW,
  ECHO_ID_ECHO,
  ECHO_ID_ECHO_G,
  ECHO_ID_ECHO_M,
  ECHO_ID_CAP,
  ECHO_ID_CONFIG,
  ECHO_ID_SETTINGS,
  ECHO_ID_BED_LEVELING,
  ECHO_ID_FADE_HEIGHT,
  ECHO_ID_TOOL_CHANGE,
  ECHO_ID_UNKNOWN_M150,
  ECHO_ID_COUNT,
} ECHO_ID;

bool dmaL1NotEmpty(uint8_t port);
void setCurrentAckSrc(uint8_t src);
void parseACK(void);
void parseRcvGcode(void);

inline bool isHostDialog() {return hostDialog;}
void setIgnoreEcho(ECHO_ID msgId, bool state);

#ifdef __cplusplus
}
#endif

#endif
