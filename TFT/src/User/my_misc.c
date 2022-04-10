#include <stdlib.h>  // first to avoid conflicts with strtod function

#include "my_misc.h"
#include "printf/printf.h"
#include <stddef.h>
#include <string.h>

uint8_t inRange(int cur, int tag , int range)
{
  if ((cur <= tag + range) && (cur >= tag - range))
    return 1;

  return 0;
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// string convert to uint8, MSB
// "2C" to 0x2C
uint8_t string_2_uint8_t(const uint8_t *string)
{
  uint8_t rtv = 0;

  for (unsigned char i = 0; i < 2; i++)
  {
    rtv <<= 4;

    if (string[i] >= '0' && string[i] <= '9')
      rtv |= string[i] - '0';
    else if (string[i] >= 'A' && string[i] <= 'F')
      rtv |= string[i] - 'A' + 0xA;
    else if (string[i] >= 'a' && string[i] <= 'f')
      rtv |= string[i] - 'a' + 0xA;
    else
      rtv |= 0;
  }

  return rtv;
}

// uint8 convert to string, MSB
// 0x2C to "2C"
uint8_t *uint8_2_string(uint8_t num, uint8_t *string)
{
  for (unsigned char i = 0; i < 2; i++)
  {
    uint8_t _4bits = (num & 0xF0) >> 4;

    if (_4bits <= 9)
      string[i] = _4bits + '0';
    else if (_4bits >= 0xA && _4bits <= 0xF)
      string[i] = _4bits - 0xA + 'A';
    else
      string[i] = 'F';

    num <<= 4;
  }

  return string;
}

// string convert to uint32, MSB
uint32_t string_2_uint32(const uint8_t *string, const uint8_t bytes_num)
{
  uint32_t rtv = 0;

  for (uint8_t i = 0; i < bytes_num; i++)
  {
    rtv <<= 8;
    rtv |= string_2_uint8_t(string + 2 * i);
  }

  return rtv;
}

// uint32 convert to string, MSB
uint8_t *uint32_2_string(uint32_t num, uint8_t bytes_num, uint8_t *string)
{
  for (uint8_t i = 0; i < bytes_num; i++)
  {
    uint8_t bit = (bytes_num - i - 1) * 8;
    uint8_t _8bits = (num & (0xFF << bit)) >> bit;

    uint8_2_string(_8bits, string + 2 * i);
  }

  return string;
}

// convert string to double (without exponential support)
double stringToDouble(char *str, char **endptr)
{
  char *p = str;
  double val = 0.0;
  int8_t sign = 1;
  uint32_t prec = 0;

  while (*p == 32 || *p == 9) { p++; }  // remove trailing whitespaces

  if (*p == '-')
  {
    sign = -1;  // set negative sign
    p++;
  }
  else if (*p == '+')
  {
    p++;  // skip positive sign
  }

  while (*p)
  {
    if (*p == '.' && prec == 0)  // increase precision first time and skip to next character
    {
      prec = 10;
      p++;
      continue;
    }

    if (*p < 48 || *p > 57)
      break;  // end on finding non-digit character

    if (prec == 0)  // read value before decimal point
    {
      val = val * 10 + *p - 48;
    }
    else  // read value after decimal point
    {
      val = val + (*p - 48) / (double)prec;
      prec = prec * 10;
    }

    p++;
  }

  if (endptr != NULL)
    *endptr = p;  // asign pointer to remaining string

  return val * sign;
}

// convert time to string with given formatting
void timeToString(char *buf, char *strFormat, uint32_t time)
{
  uint8_t hour = HOURS(time);
  uint8_t min = MINUTES(time);
  uint8_t sec = SECONDS(time);

  sprintf(buf, strFormat, hour, min, sec);
}

// strip out any leading " ", "/" or ":" character that might be in the string
const char *stripHead(const char *str)
{
  // example: ":    /test/cap2.gcode\n" -> "test/cap2.gcode\n"

  while (*str == ' ' || *str == '/' || *str == ':')
  {
    str++;
  }

  return str;
}

// strip out any trailing checksum that might be in the string
void stripChecksum(char *str)
{
  // examples:
  //
  // "/test/cap2.gcode  *36\n\0" -> "/test/cap2.gcode"
  // "/test/cap2.gcode  \n\0" -> "/test/cap2.gcode"

  char *strPtr = strrchr(str, '*');  // e.g. "/test/cap2.gcode  *36\n\0" -> "*36\n\0"

  if (strPtr == NULL)
    strPtr = str + strlen(str);      // e.g. "/test/cap2.gcode  \n\0" -> "\0"

  while (strPtr != str)
  {
    // e.g. "*36\n\0" -> " *36\n\0"
    // e.g. "\0" -> "\n\0"
    //
    --strPtr;

    if (*strPtr != ' ' && *strPtr != '\t' && *strPtr != '\n' && *strPtr != '\r')
    {
      strPtr++;  // next char has to be set to "\0"
      break;
    }
  }

  // e.g. "  *36\n\0" -> "\0 *36\n\0"
  // e.g. "  \n\0" -> "\0 \n\0"
  //
  *strPtr = '\0';
}

uint8_t getChecksum(char *str)
{
  uint8_t checksum = 0;

  while (*str != '\0')
  {
    checksum ^= *(str++);
  }

  return checksum;
}

bool validateChecksum(char *str)
{
  char *strPtr = strrchr(str, '*');  // e.g. "N1 G28*18\n\0" -> "*18\n\0"

  if (strPtr == NULL)
    return false;

  uint8_t checksum = 0;
  uint8_t value = strtol(&strPtr[1], NULL, 10);

  while (strPtr != str)
  {
    checksum ^= *(--strPtr);
  }

  return (checksum == value ? true : false);
}
