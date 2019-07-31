#ifndef _USB_LOG_H_
#define _USB_LOG_H_

  #include "stdio.h"
  
  #if 0
    #define LOG_printf(...) printf(__VA_ARGS__);printf("\r\n")
  #else
    #define LOG_printf(...) ((void)0)
  #endif
    
#endif
