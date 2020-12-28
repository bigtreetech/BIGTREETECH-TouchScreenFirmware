#ifndef _FAN_H_
#define _FAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void menuFan(void);

typedef struct
{
    uint8_t cur;
    uint8_t set;
} LASTFAN;


#ifdef __cplusplus
}
#endif

#endif
