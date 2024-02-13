#include <Mainboard_CmdControl.h>
#include <includes.h>

// line number of last command properly processed by mainboard,
// base line number and line number of last command sent by TFT respectively.
// Required COMMAND_CHECKSUM feature enabled in TFT
static uint32_t cmd_line_number_ok = NO_LINE_NUMBER;
static uint32_t cmd_line_number_base = 0;
static uint32_t cmd_line_number = 0;

uint32_t getCmdLineNumberOk(void)
{
  return cmd_line_number_ok;
}

void setCmdLineNumberOk(const uint32_t lineNumber)
{
  cmd_line_number_ok = lineNumber;
}

uint32_t getCmdLineNumberBase(void)
{
  return cmd_line_number_base;
}

void setCmdLineNumberBase(const uint32_t lineNumber)
{
  cmd_line_number_base = cmd_line_number = lineNumber;
}

uint32_t getCmdLineNumber(void)
{
  return cmd_line_number;
}

uint32_t addCmdLineNumberAndChecksum(CMD cmd, uint8_t cmdIndex, uint8_t * cmdLen)
{
  CMD plainCmd;  // plain command (with no line number and checksum, if any)

  strcpy(plainCmd, &cmd[cmdIndex]);  // e.g. "N1 G28*18\n" -> "G28*18\n"
  stripCmdChecksum(plainCmd);        // e.g. "G28*18\n" -> "G28"

  if (strlen(plainCmd) + 16 > CMD_MAX_SIZE)  // we consider extra bytes for line number, checksum, "\n" and "\0"
  {
    addNotification(DIALOG_TYPE_ERROR, "Cmd too long", cmd, true);

    return NO_LINE_NUMBER;
  }

  cmd_line_number++;  // next command line number to be used

  sprintf(cmd, "N%lu %s", cmd_line_number, plainCmd);        // e.g. "G28" -> "N2 G28"
  sprintf(strchr(cmd, '\0'), "*%u\n", getCmdChecksum(cmd));  // e.g. "N2 G28" -> "N2 G28*17\n"

  *cmdLen = strlen(cmd);

  return cmd_line_number;
}

const char * stripCmdHead(const CMD cmd)
{
  // example: ":    /test/cap2.gcode\n" -> "test/cap2.gcode\n"

  while (*cmd == ' ' || *cmd == '/' || *cmd == ':')
  {
    cmd++;
  }

  return cmd;
}

void stripCmdChecksum(CMD cmd)
{
  // examples:
  //
  // "/test/cap2.gcode  *18\n\0" -> "/test/cap2.gcode"
  // "/test/cap2.gcode  \n\0" -> "/test/cap2.gcode"

  char * cmdPtr = strrchr(cmd, '*');  // e.g. "/test/cap2.gcode  *18\n\0" -> "*18\n\0"

  if (cmdPtr == NULL)
    cmdPtr = cmd + strlen(cmd);       // e.g. "/test/cap2.gcode  \n\0" -> "\0"

  while (cmdPtr != cmd)
  {
    // e.g. "*18\n\0" -> " *18\n\0"
    // e.g. "\0" -> "\n\0"
    //
    --cmdPtr;

    if (*cmdPtr != ' ' && *cmdPtr != '\t' && *cmdPtr != '\n' && *cmdPtr != '\r')
    {
      cmdPtr++;  // next char has to be set to "\0"
      break;
    }
  }

  // e.g. "  *18\n\0" -> "\0 *18\n\0"
  // e.g. "  \n\0" -> "\0 \n\0"
  //
  *cmdPtr = '\0';
}

uint8_t getCmdChecksum(const CMD cmd)
{
  uint8_t checksum = 0;

  while (*cmd != '\0')
  {
    checksum ^= *(cmd++);
  }

  return checksum;
}

bool validateCmdChecksum(const CMD cmd)
{
  char * cmdPtr = strrchr(cmd, '*');  // e.g. "N1 G28*18\n\0" -> "*18\n\0"

  if (cmdPtr == NULL)
    return false;

  uint8_t checksum = 0;
  uint8_t value = strtol(&cmdPtr[1], NULL, 10);

  while (cmdPtr != cmd)
  {
    checksum ^= *(--cmdPtr);
  }

  return (checksum == value ? true : false);
}

const char * parseM118(CMD cmd, bool * hasE, bool * hasA)
{
  stripCmdChecksum((char *) stripCmdHead(cmd));

  *hasE = false;
  *hasA = false;

  for (uint8_t i = 3; i--;)
  {
    // A1, E1 and Pn are always parsed out
    if (!(((cmd[0] == 'A' || cmd[0] == 'E') && cmd[1] == '1') || (cmd[0] == 'P' && NUMERIC(cmd[1]))))
      break;

    switch (cmd[0])
    {
      case 'A':
        *hasA = true;
        break;

      case 'E':
        *hasE = true;
        break;
    }

    cmd += 2;

    while (*cmd == ' ')
    {
      ++cmd;
    }
  }

  return cmd;
}
