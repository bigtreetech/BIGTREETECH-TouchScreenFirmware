#include "comment.h"
#include "includes.h"

#define TOKEN_DELIMITERS " :=_"   // possible delimiters found in the comments of the G-code file
#define KEYWORD_INTIALS "lLrRtT"  // the initials of the searched keywords, add new initials for new keywords

static bool slicerTimePresence = false;

char gCodeCommentLine[COMMENT_MAX_CHAR] = {0};

void setTimeFromSlicer(bool present)
{
  slicerTimePresence = present;
}

void parseComment(void)
{
  if (gCodeCommentLine[0] == '\0')
    return;

  // check for words starting with "l", "L", "r", "R", "t" or "T".
  // It is done so for speed purposes, it is a waste of MCU cycles to extract
  // tokens, convert them to lower case and check if they are among the known
  // keywords if they do not start with the above mentioned letters
  //
  if (strchr(KEYWORD_INTIALS, gCodeCommentLine[0]) != NULL)
  {
    char * token;
    uint32_t token_value;

    token = strlwr(strtok(gCodeCommentLine, TOKEN_DELIMITERS));

    // check for "layer" keyword in comment (layer number or layer count)
    if (strcmp(token, "layer") == 0)
    {
      if ((token = strtok(NULL, TOKEN_DELIMITERS)) != NULL)
      {
        strlwr(token);

        if (strcmp(token, "count") == 0)  // check if next word is "count"
        {
          if ((token = strtok(NULL, TOKEN_DELIMITERS)) != NULL)  // get the layer number
          {
            token_value = strtoul(token, NULL, 0);

            if (token_value != 0)
              setPrintLayerCount(token_value);
          }
        }
        else if (NUMERIC(token[0]))  // check if a number is found
        {
          token_value = strtoul(token, NULL, 0);

          // "token_value == 0" for object by object printing, when print goes to the next object
          // if there is "layer 0" add an offset of 1 (avoiding using an offset variable)
          setPrintLayerNumber(((token_value == 0) || (getPrintLayerNumber() == token_value)) ? token_value + 1 : token_value);
        }
      }
    }

    // check for "time" keyword in comment to retrieve total or elapsed time, Cura specific
    else if (strcmp(token, "time") == 0 && slicerTimePresence == false)  // check if first word is "time"
    {
      if ((token = strtok(NULL, TOKEN_DELIMITERS)) != NULL)
      {
        strlwr(token);

        if (strcmp(token, "elapsed") == 0 && getPrintExpectedTime() > 0)  // check if next word is "elapsed"
        {
          if ((token = strtok(NULL, TOKEN_DELIMITERS)) != NULL)  // get the elapsed time in seconds
          {
            setPrintRemainingTime(getPrintExpectedTime() - strtoul(token, NULL, 0));
          }
        }
        else if (NUMERIC(token[0]))  // check if a number is found
        {
          token_value = strtoul(token, NULL, 0);  // get the time in seconds

          setPrintExpectedTime(token_value);
          setPrintRemainingTime(token_value);

          if (getPrintProgressSource() < PROG_TIME && infoSettings.prog_source == 1)
            setPrintProgressSource(PROG_TIME);
        }
      }
    }

    // check for "remaining" keyword in comment to retrieve remaining time, IdeaMaker specific
    else if (strcmp(token, "remaining") == 0 && slicerTimePresence == false)  // check if first word is "remaining"
    {
      if ((token = strtok(NULL, TOKEN_DELIMITERS)) != NULL)
      {
        strlwr(token);

        if (strcmp(token, "time") == 0)  // check if next word is "time"
        {
          if ((token = strtok(NULL, TOKEN_DELIMITERS)) != NULL)  // get the remaining time in seconds
          {
            setPrintRemainingTime(strtoul(token, NULL, 0));

            if (getPrintProgressSource() < PROG_TIME && infoSettings.prog_source == 1)
              setPrintProgressSource(PROG_TIME);
          }
        }
      }
    }
  }

  gCodeCommentLine[0] = '\0';
}
