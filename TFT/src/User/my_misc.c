#include <stdlib.h>  // first to avoid conflicts with strtod() function

#include "my_misc.h"
#include "printf/printf.h"
#include <stddef.h>

#define CRC_POLY 0xA001

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

// calculate CRC16 checksum
uint32_t calculateCRC16(const uint8_t *data, uint32_t length)
{
  uint16_t crc = 0xFFFF;
  uint32_t i;
  for (i = 0; i < length; i++)
  {
    crc = (crc ^ data[i]) & 0xFFFF;
    for (uint8_t j = 0; j < 8; j++)
    {
      if (crc & 1)
      {
        crc = (crc >> 1) ^ CRC_POLY;
      }
      else
      {
        crc = crc >> 1;
      }
    }
  }
  return crc;
}

// string convert to uint8, MSB ("2C" to 0x2C)
uint8_t string_2_uint8(const uint8_t *str)
{
  uint8_t rtv = 0;

  for (unsigned char i = 0; i < 2; i++)
  {
    rtv <<= 4;

    if (str[i] >= '0' && str[i] <= '9')
      rtv |= str[i] - '0';
    else if (str[i] >= 'A' && str[i] <= 'F')
      rtv |= str[i] - 'A' + 0xA;
    else if (str[i] >= 'a' && str[i] <= 'f')
      rtv |= str[i] - 'a' + 0xA;
    else
      rtv |= 0;
  }

  return rtv;
}

// uint8 convert to string, MSB (0x2C to "2C")
uint8_t *uint8_2_string(uint8_t num, uint8_t *str)
{
  for (unsigned char i = 0; i < 2; i++)
  {
    uint8_t _4bits = (num & 0xF0) >> 4;

    if (_4bits <= 9)
      str[i] = _4bits + '0';
    else if (_4bits >= 0xA && _4bits <= 0xF)
      str[i] = _4bits - 0xA + 'A';
    else
      str[i] = 'F';

    num <<= 4;
  }

  return str;
}

// string convert to uint32, MSB
uint32_t string_2_uint32(const uint8_t *str, const uint8_t bytes_num)
{
  uint32_t rtv = 0;

  for (uint8_t i = 0; i < bytes_num; i++)
  {
    rtv <<= 8;
    rtv |= string_2_uint8(str + 2 * i);
  }

  return rtv;
}

// uint32 convert to string, MSB
uint8_t *uint32_2_string(uint32_t num, uint8_t bytes_num, uint8_t *str)
{
  for (uint8_t i = 0; i < bytes_num; i++)
  {
    uint8_t bit = (bytes_num - i - 1) * 8;
    uint8_t _8bits = (num & (0xFF << bit)) >> bit;

    uint8_2_string(_8bits, str + 2 * i);
  }

  return str;
}

// convert time to string with given formatting
void time_2_string(char *buf, char *str_format, uint32_t time)
{
  uint8_t hour = HOURS(time);
  uint8_t min = MINUTES(time);
  uint8_t sec = SECONDS(time);

  sprintf(buf, str_format, hour, min, sec);
}

// light weight strtod() function without exponential support.
// Convert string to double (without exponential support)
double strtod_ligth(char *str, char **endptr)
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

// light weight and safe strncpy() function with padding:
// - copy "src" to "dest" for a maximum of "n-1" characters
// - if null terminating character is found in "src" the rest in "dest" is padded with '\0'
// - "dest" always ends with '\0'
void strncpy_pad(char *dest, const char *src, size_t n)
{
  // if "src" is not NULL, proceed first with the copy.
  // Otherwise, proceed only padding "dest" with '\0'
  if (src != NULL)
  {
    while (n > 1 && *src != '\0')
    {
      *dest++ = *src++;
      n--;
    }
  }

  memset(dest, '\0', n);  // NOTE: safe even in case value 0 was passed for "n" (memset() function will do nothing)
}

// light weight and safe strncpy() function without padding:
// - copy "src" to "dest" for a maximum of "n-1" characters
// - if null terminating character is found in "src" the copy stops there
// - "dest" always ends with '\0'
void strncpy_no_pad(char *dest, const char *src, size_t n)
{
  // if "src" is not NULL, proceed with the copy
  if (src != NULL)
  {
    while (n > 1 && *src != '\0')
    {
      *dest++ = *src++;
      n--;
    }
  }

  if (n != 0)  // safe in case value 0 was passed for "n"
    *dest = '\0';
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

const char *parseM118(char *str, bool *hasE, bool *hasA)
{
  stripChecksum(str);
  str = (char *) stripHead(str);

  *hasE = false;
  *hasA = false;

  for (uint8_t i = 3; i--;)
  {
    // A1, E1 and Pn are always parsed out
    if (!(((str[0] == 'A' || str[0] == 'E') && str[1] == '1') || (str[0] == 'P' && NUMERIC(str[1]))))
      break;

    switch (str[0])
    {
      case 'A':
        *hasA = true;
        break;

      case 'E':
        *hasE = true;
        break;
    }

    str += 2;

    while (*str == ' ')
    {
      ++str;
    }
  }

  return str;
}
