#ifndef _PARSEACK_H_
#define _PARSEACK_H_

#include "stdint.h"
#include "Configuration.h"

static const char errormagic[]         = "Error:";
static const char echomagic[]          = "echo:";
static const char unknowmagic[]        = "Unknown command:";
static const char bsdprintingmagic[]   = "SD printing byte";
static const char bsdnoprintingmagic[] = "Not SD printing";

#define ACK_MAX_SIZE 2048

typedef enum                           // popup message types available to display an echo message
{
  ECHO_POPUP_NONE = 0,                 // no popup. The echo message is silently discarded
  ECHO_POPUP_REMINDER,                 // reminder popup. A reminder popup is displayed. A user interaction is needed
  ECHO_POPUP_NOTIFICATION,             // notification popup. A notification popup is displayed for few seconds. No user interaction is needed
} _ECHO_POPUP_TYPE;

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
} _ECHO_MSG_ID;

/*
  It retrives the current attribute values for the specified msgId

  input:   msgId
  
  ouput:   curPopupType
           curBuzzerEnabled

  returns: true, if msgId is found and its attribute values are retrieved in curPopupType and curBuzzerEnabled
           false, otherwise
*/
bool getKnownEchoParam(_ECHO_MSG_ID msgId, _ECHO_POPUP_TYPE *curPopupType, bool *curBuzzerEnabled);

/*
  It sets the new attribute values for the specified msgId

  input:   msgId
           newPopupType
           newBuzzerEnabled

  returns: true, if msgId is found and its attribute values are set with newPopupType and newBuzzerEnabled
           false, otherwise
*/
bool setKnownEchoParam(_ECHO_MSG_ID msgId, _ECHO_POPUP_TYPE newPopupType, bool newBuzzerEnabled);

void setCurrentAckSrc(uint8_t src);
void parseACK(void);
void parseRcvGcode(void);

#endif
