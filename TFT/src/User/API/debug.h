#ifndef _DEBUG_H_
#define _DEBUG_H_

#if defined(SERIAL_DEBUG_ENABLED) && defined(SERIAL_DEBUG_PORT)
    // Redirect dbg_printf to TinyPrintf (printf_) which itself write to Serial_Putchar(SERIAL_DEBUG_PORT, ...)
    #define dbg_printf(...) printf_(__VA_ARGS__)
    #define dbg_print(str) Serial_Puts(SERIAL_DEBUG_PORT, str)
#else
    #define dbg_printf(...)
    #define dbg_print(str)
#endif

#endif
