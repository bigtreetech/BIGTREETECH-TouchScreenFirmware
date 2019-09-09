#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "variants.h"
#include "parseACK.h"

void Serial_DMAReEnable(void);
    
void Serial_Config(u32 baud);
void Serial_DeConfig(void);
void Serial_Puts(char *s);

#endif 




















