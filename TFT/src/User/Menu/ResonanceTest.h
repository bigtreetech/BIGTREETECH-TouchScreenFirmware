#ifndef _RESONANCE_TEST_H_
#define _RESONANCE_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// resonance test (M495 / M496) is available only if the mainboard reports a feature it depends on
bool hasResonanceTest(void);

void menuResonanceTest(void);

#ifdef __cplusplus
}
#endif

#endif
