#include "comment.h"
#include "includes.h"
#include <string.h>

#define TOKEN_DELIMITERS " :=_"
#define HIGH_TO_LOW_CASE     32  // 'a' - 'A'

char gCodeCommentLine[COMMENT_MAX_CHAR] = {0};
bool slicerTimePresence = false;

void setTimeFromSlicer(bool present)
{
  slicerTimePresence = present;
}

void parseComment(void)
{
  if (gCodeCommentLine[0] == '\0')
    return;

  char * temp_char;
  uint32_t temp_value = 0;

  // check for words starting with "l" or "L"
  if (gCodeCommentLine[0] == 'l' || gCodeCommentLine[0] == 'L')
  {
    temp_char = strtok(gCodeCommentLine, TOKEN_DELIMITERS);
    strlwr(temp_char);

    if (strcmp(temp_char, "layer") == 0)
    { // check for "layer" keyword in comment (layer number or layer count)
      temp_char = strtok(NULL, TOKEN_DELIMITERS);
      strlwr(temp_char);

      if (strcmp(temp_char, "count") == 0)  // check if next word is "count"
      {
        temp_char = strtok(NULL, TOKEN_DELIMITERS);
        temp_value = strtoul(temp_char, NULL, 0);

        if (temp_value != 0)
          setPrintLayerCount(temp_value);
      }
      else if (NUMERIC(temp_char[0]))  // check if a number is found
      {
        temp_value = strtoul(temp_char, NULL, 0);
        // "temp_value == 0" for object by object printing, when print goes to the next object
        // if there is "layer 0" add an offset of 1 (avoiding using an offset variable)
        setPrintLayerNumber(((temp_value == 0) || (getPrintLayerNumber() == temp_value)) ? temp_value + 1: temp_value);
      }
    }
    // continue here with "else if" for another token that starts with "l" or "L"
  }

  // check for words starting with "t" or "T"
  else if (gCodeCommentLine[0] == 't' || gCodeCommentLine[0] == 'T')
  {
    temp_char = strtok(gCodeCommentLine, TOKEN_DELIMITERS);
    strlwr(temp_char);

    // check for "time" keyword in comment to retrieve total or elapsed time, Cura specific
    if (strcmp(temp_char, "time") == 0 && slicerTimePresence == false)  // check if first word is "time"
    {
      temp_char = strtok(NULL, TOKEN_DELIMITERS);
      strlwr(temp_char);

      if (strcmp(temp_char, "elapsed") == 0 && getPrintExpectedTime() > 0)  // check if next word is "elapsed"
      {
        temp_char = strtok(NULL, TOKEN_DELIMITERS);
        temp_value = strtoul(temp_char, NULL, 0);  // get the elapsed time in seconds
        setPrintRemainingTime(getPrintExpectedTime() - temp_value);
      }
      else if (temp_char[0] >= '0' && temp_char[0] <= '9')  // check if a number is found
      {
        setPrintExpectedTime(strtoul(temp_char, NULL, 0));
        setPrintRemainingTime(getPrintExpectedTime());

        if (getPrintProgSource() < PROG_TIME && infoSettings.prog_source == 1)
          setPrintProgSource(PROG_TIME);
      }
    }
    // continue here with "else if" for another token that starts with "t" or "T"
  }

  // check for words starting with "r" or "R"
  else if (gCodeCommentLine[0] == 'r' || gCodeCommentLine[0] == 'R')
  {
    temp_char = strtok(gCodeCommentLine, TOKEN_DELIMITERS);
    strlwr(temp_char);

    // check for "remaining" keyword in comment to retrieve remaining time, IdeaMaker specific
    if (strcmp(temp_char, "remaining") == 0 && slicerTimePresence == false)  // check if first word is "remaining"
    {
      temp_char = strtok(NULL, TOKEN_DELIMITERS);
      strlwr(temp_char);

      if (strcmp(temp_char, "time") == 0)  // check if next word is "time"
      {
        temp_char = strtok(NULL, TOKEN_DELIMITERS);
        temp_value = strtoul(temp_char, NULL, 0);  // get the remaining time in seconds
        setPrintRemainingTime(temp_value);

        if (getPrintProgSource() < PROG_TIME && infoSettings.prog_source == 1)
          setPrintProgSource(PROG_TIME);
      }
    }
    // continue here with "else if" for another token that starts with "r" or "R"
  }

  gCodeCommentLine[0] = '\0';
}
