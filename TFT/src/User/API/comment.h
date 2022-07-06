#ifndef _COMMENT_H_
#define _COMMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define COMMENT_MAX_CHAR 100

extern char gCodeCommentLine[COMMENT_MAX_CHAR];

void setTimeFromSlicer(bool present);
void parseComment(void);

#ifdef __cplusplus
}
#endif

#endif
