#ifndef _BLTOUCH_H_
#define _BLTOUCH_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  HS_OFF = 0,
  HS_ON,
  HS_DISABLED
} BLT_HS_MODE;

void setHSmode(BLT_HS_MODE hsMode);
void menuBLTouch(void);

#ifdef __cplusplus
}
#endif

#endif
