#include "includes.h"

static uint32_t line_number = 0;

void rrfSendCmd(const char * cmd_ptr)
{
  char rrfCmd[CMD_MAX_SIZE];
  char * rrfCmd_ptr = rrfCmd;
  uint8_t checksum = 0;

  sprintf(rrfCmd, "N%lu %s", line_number++, cmd_ptr);

  // calculate checksum
  while (*rrfCmd_ptr != '\n')
  {
    checksum ^= *rrfCmd_ptr++;
  }

  // add checksum and finalize formatting the RRF command
  sprintf(rrfCmd_ptr, "*%u\n", checksum);

  // send the command to the serial port
  Serial_Put(SERIAL_PORT, rrfCmd);
}
