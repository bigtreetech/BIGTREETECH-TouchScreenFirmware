#ifndef _COMMENT_H_
#define _COMMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define COMMENT_MAX_CHAR 100

typedef struct
{
  char content[COMMENT_MAX_CHAR];
  bool handled;
} COMMENT;

extern COMMENT gCode_comment;

void parseComment();
void setM73_presence(bool present);

#ifdef __cplusplus
}
#endif

#endif
