//TG MODIFIED BY T.GIOIOSA
#ifndef _DUSTCONTROL_H_
#define _DUSTCONTROL_H_


extern const char *const vacuumID[MAX_VACUUM_COUNT];
extern const char *const vacuumDisplayID[MAX_VACUUM_COUNT];
extern const char *const vacuumCmd[MAX_VACUUM_COUNT];

extern uint8_t vacuumState;

void menuVacuum(void);
uint8_t getVacuumState(void);
void loopVacuum(void);
void vacuum_set (uint8_t state);
#endif
