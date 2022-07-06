#ifndef _MPC_H_
#define _MPC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
  REQUESTED = 0,
  STARTED,
  ONGOING,
  DONE,
} MPC_STATUS;

typedef enum
{
  NO_RESULT = 0,
  INTERRUPTED,
  FINISHED,
} MPC_RESULT;

void menuMPC(void);
void setMpcTuningStatus(MPC_STATUS status);
MPC_STATUS getMpcTuningStatus(void);
void setMpcTuningResult(MPC_RESULT result);
void setMpcHeaterPower(uint8_t index, uint8_t power);
void setMpcFilHeatCapacity(uint8_t index, float capacity);
bool hasMPC(void);

#ifdef __cplusplus
}
#endif

#endif
