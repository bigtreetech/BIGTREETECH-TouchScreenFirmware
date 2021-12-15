#include "RRFSendCmd.h"
#include "Serial.h"
#include <stdio.h>

static uint8_t n_sent = 0;
static uint32_t line_number = 0;
static uint8_t checksum = 0;

void sendCharAndChecksum(const char c)
{
  checksum ^= c;
  Serial_Putchar(SERIAL_PORT, c);
  n_sent++;
}

void sendChar(const char c)
{
  if (c == '\n')
  {
    if (n_sent != 0)
    {
      Serial_Putchar(SERIAL_PORT, '*');
      char digit0 = checksum % 10 + '0';
      checksum /= 10;
      char digit1 = checksum % 10 + '0';
      checksum /= 10;
      if (checksum != 0)
      {
        Serial_Putchar(SERIAL_PORT, checksum + '0');
      }
      Serial_Putchar(SERIAL_PORT, digit1);
      Serial_Putchar(SERIAL_PORT, digit0);
    }
    Serial_Putchar(SERIAL_PORT, c);
    n_sent = 0;
  }
  else
  {
    if (n_sent == 0)
    {
      char number[11];
      checksum = 0;
      sendCharAndChecksum('N');
      snprintf(number, 11, "%lu", line_number++);
      rrfSendCmd(number);
      sendCharAndChecksum(' ');
    }
    sendCharAndChecksum(c);
  }
}

void rrfSendCmd(const char* cmd_ptr)
{
  while (*cmd_ptr != 0)
  {
    sendChar(*cmd_ptr++);
  }
}
