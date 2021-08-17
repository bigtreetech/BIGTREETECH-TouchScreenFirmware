#include "comment.h"
#include "includes.h"
#include <string.h>

#define TOKEN_DELIMITERS " :=_"

COMMENT gCode_comment = {0, true};
static uint16_t layerNumber = 0;
static uint16_t layerCount = 0;
static uint32_t totalTime = 0;

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

void setLayerNumber(uint16_t layer_number)
{
  layerNumber = layer_number;
}

void setTotalTime(uint32_t time)
{
  totalTime = time;
}

void parseComment()
{
    char * temp_char;
    uint32_t temp_value = 0;
    if (gCode_comment.handled == true)
      return;

    // check for "layer" keyword in comment (layer number or layer count)
    if (gCode_comment.content[0] == 'l' || gCode_comment.content[0] == 'L')
    {
      temp_char = strtok(gCode_comment.content, TOKEN_DELIMITERS);
      lowerCase(temp_char);
      if (strcmp(temp_char, "layer") == 0)
      {
        temp_char = strtok(NULL, TOKEN_DELIMITERS);
        lowerCase(temp_char);
        if (strcmp(temp_char, "count") == 0)  // check if next word is "count"
        {
          temp_char = strtok(NULL, TOKEN_DELIMITERS);
          temp_value = strtoul(temp_char, NULL, 0);
          if (temp_value != 0)
            layerCount = temp_value;
        }
        else if (temp_char[0] >= '0' && temp_char[0] <= '9')  // check if a number is found
        {
          temp_value = strtoul(temp_char, NULL, 0);

          // if there is "layer 0" add an offset of 1 (avoiding using an offset variable)
          layerNumber = (layerNumber == temp_value) ? temp_value + 1: temp_value;
        }
      }
      // continue here with "else if" for another token that starts with "l" or "L"
    }

    // check for "time" keyword in comment to retrieve total or elapsed time
    else if (gCode_comment.content[0] == 't' || gCode_comment.content[0] == 'T')
    {
      temp_char = strtok(gCode_comment.content, TOKEN_DELIMITERS);
      lowerCase(temp_char);
      if (strcmp(temp_char, "time") == 0)
      {
        temp_char = strtok(NULL, TOKEN_DELIMITERS);
        lowerCase(temp_char);
        if (strcmp(temp_char, "elapsed") == 0 && totalTime > 0)  // check if next word is "elapsed"
        {
          temp_char = strtok(NULL, TOKEN_DELIMITERS);
          temp_value = strtoul(temp_char, NULL, 0);  // get the elapsed time in seconds
          setPrintRemainingTime(totalTime - temp_value);
        }
        else if (temp_char[0] >= '0' && temp_char[0] <= '9')  // check if a number is found
        {
          totalTime = strtoul(temp_char, NULL, 0);
          setPrintRemainingTime(totalTime);
        }
      }
      // continue here with "else if" for another token that starts with "t" or "T"
    }

    gCode_comment.handled = true;
}
