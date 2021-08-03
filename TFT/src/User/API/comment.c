#include "comment.h"
#include "includes.h"
#include <string.h>

#define TOKEN_DELIMITERS " :=_"

COMMENT gCode_comment = {0, true};
static uint16_t layerNumber = 0;
static uint16_t layerCount = 0;

void lowerCase (char * tempChar)
{
    for (uint8_t i=0; i < strlen(tempChar); i++)
    {
      if (tempChar[i] >= 'A' && tempChar[i] <= 'Z')
        tempChar[i] = tempChar[i] -'A' + 'a';
    }
}

uint16_t getLayerNumber()
{
  return layerNumber;
}

uint16_t getLayerCount()
{
  return layerCount;
}

void parseComment()
{
    char * temp_char;
    uint16_t temp_value = 0;
    if (gCode_comment.handled == true)
      return;

    // check for "layer" keyword in comment (layer number or layer count)
    if (gCode_comment.content[0] == 'l' || gCode_comment.content[0] == 'L')
    {
        temp_char = strtok(gCode_comment.content, TOKEN_DELIMITERS);
        if (temp_char != NULL)
        {
          lowerCase(temp_char);
        }
        if (strcmp(temp_char, "layer") == 0)
        {
          temp_char = strtok(NULL, TOKEN_DELIMITERS);
          lowerCase(temp_char);
          if (strcmp(temp_char, "count") == 0)
          {
            temp_char = strtok(NULL, TOKEN_DELIMITERS);
            temp_value = strtoul(temp_char, NULL, 0);
            if (temp_value != 0)
              layerCount = temp_value;
          }
          else
          {
            temp_value = strtoul(temp_char, NULL, 0);
            if (temp_value != 0)
              layerNumber = temp_value;
          }
        }
    }

    gCode_comment.handled = true;
}