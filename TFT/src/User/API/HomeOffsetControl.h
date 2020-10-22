#ifndef _HOME_OFFSET_CONTROL_H_
#define _HOME_OFFSET_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Set Z offset value */
float homeOffsetSetValue(float value);

/* Get current Z offset value */
float homeOffsetGetValue(void);

#ifdef __cplusplus
}
#endif

#endif
