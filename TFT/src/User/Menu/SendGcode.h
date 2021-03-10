#ifndef _SEND_GCODE_H_
#define _SEND_GCODE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  TERMINAL_GCODE,
  TERMINAL_ACK,
} TERMINAL_SRC;

void menuTerminal(void);
void terminalCache(char *stream, TERMINAL_SRC src);

#ifdef __cplusplus
}
#endif

#endif
