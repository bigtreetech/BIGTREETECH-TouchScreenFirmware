#ifndef _MAINBOARD_CMD_CONTROL_
#define _MAINBOARD_CMD_CONTROL_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <Mainboard_CmdHandler.h>  // for CMD

#define NO_LINE_NUMBER 0xFFFFFFFF;                     // no processed line number

uint32_t getCmdLineNumberOk(void);                     // get line number of last command properly processed by mainboard
void setCmdLineNumberOk(const uint32_t lineNumber);    // set line number of last command properly processed by mainboard
uint32_t getCmdLineNumberBase(void);                   // get base line number
void setCmdLineNumberBase(const uint32_t lineNumber);  // set base line number
uint32_t getCmdLineNumber(void);                       // get line number of last command sent by TFT

// add line number and checksum to the provided command (cmd and cmdLen are properly set)
// and return the used line number
uint32_t addCmdLineNumberAndChecksum(CMD cmd, uint8_t cmdIndex, uint8_t * cmdLen);

const char * stripCmdHead(const CMD cmd);  // strip out any leading " ", "/" or ":" character that might be in the command
void stripCmdChecksum(CMD cmd);            // strip out any trailing checksum that might be in the command
uint8_t getCmdChecksum(const CMD cmd);     // return checksum for the provided command
bool validateCmdChecksum(const CMD cmd);   // validate checksum for the provided command

const char * parseM118(CMD cmd, bool * hasE, bool * hasA);  // parse M118 gcode

#ifdef __cplusplus
}
#endif

#endif
