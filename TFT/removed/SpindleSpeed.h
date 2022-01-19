//TG MODIFIED BY T.GIOIOSA

#ifndef __SPINDLE_SPEED_H__
#define __SPINDLE_SPEED_H__

//static bool  spindle_speed_update_waiting; 
//static bool  spindle_speed_send_waiting[MAX_TOOL_COUNT];
uint32_t nextSpindleSpeedCheckTime;


#define SPINDLE_SPEED_QUERY_FAST_SECONDS 1  // 
#define SPINDLE_SPEED_QUERY_SLOW_SECONDS 3  // 

typedef enum {
  SP_WAIT_NONE = 0,
  SP_WAIT_HEATING,
  SP_WAIT_COOLING_HEATING,
}SPINDLE_SPEED_WAIT;

enum
{
  SP_TOOL0 = 0,    //TG 2/4/21  reduced tools
  SP_TOOL1,
  SP_BED,
  SP_CHAMBER,
};

typedef struct
{
  int16_t current,
          target;
  SPINDLE_SPEED_WAIT waiting;
}_SPINDLE_SPEED;

typedef struct
{
  union {
    struct {
      _SPINDLE_SPEED hotend[MAX_SPINDLE_COUNT];
      _SPINDLE_SPEED bed;
      _SPINDLE_SPEED chamber;
    };
    _SPINDLE_SPEED T[MAX_SPINDLE_COUNT];
  };
  uint8_t      toolIndex;  //
}SPINDLE;


extern char * spindleID[];
extern char * spindleDisplayID[];
extern char * spindleCmd[];
extern char * spindleWaitCmd[];


void spindleSpeedSetUpdateSeconds(uint8_t seconds);
uint8_t spindleSpeedGetUpdateSeconds(void);
void spindleSpeedSyncUpdateSeconds(uint8_t seconds);
void spindleSpeedSetUpdateWaiting(bool isWaiting);
void spindleSpeedSetSendWaiting(uint8_t index, bool isWaiting);
bool spindleSpeedGetSendWaiting(uint8_t index);
void updateNextSpindleSpeedCheckTime(void);
void loopCheckSpindleSpeed(void);


void spindleSpeedSetCurrentTool(uint8_t tool);
uint8_t spindleSpeedGetCurrentTool(void);
uint8_t spindleSpeedGetCurrentHotend(void);
bool spindleSpeedIsValid(uint8_t index);

bool spindleSpeedGetIsWaiting(uint8_t index);
bool spindleSpeedHasWaiting(void);
void spindleSpeedSetIsWaiting(uint8_t index,SPINDLE_SPEED_WAIT isWaiting);
void spindleSpeedClearIsWaiting(void);
void updateNextspindleSpeedCheckTime(void);

#endif