#include "parseACKJson.hpp"
#include "includes.h"

/*
 * Parses a structure like follows:
 * {
 *    "status": "I",
 *    "heaters": [ 21.7, 21.3 ],
 *    "active": [ 0, 0 ],
 *    "standby": [ 0, 0 ],
 *    "hstat": [ 0, 0 ],
 *    "pos": [ 67, 141, 7.12 ],
 *    "machine": [ 67, 141, 7.12 ],
 *    "sfactor": 100,
 *    "efactor": [ 100 ],
 *    "babystep": 0,
 *    "tool": -1,
 *    "probe": "0",
 *    "fanPercent": [ 0, 100, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 ],
 *    "fanRPM": [ -1, -1, -1 ],
 *    "homed": [ 0, 0, 0 ],
 *    "msgBox.mode": -1
 *  }
 */
void ParseACKJsonParser::endDocument()
{
  requestCommandInfo.inJson = false;
}

void ParseACKJsonParser::value(const char *value)
{
  switch (state)
  {
    case fan_percent:
      if (index != 0 && index <= infoSettings.fan_count) // index 0 is an alias for default tool fan
      {
        fanSetPercent(index - 1, strtod((char *)value, NULL) + 0.5f);
      }
      break;
    case efactor:
      if (index == heatGetCurrentTool())
      {
        speedSetCurPercent(1, strtod((char *)value, NULL));
      }
      break;
    case sfactor:
      speedSetCurPercent(0, strtod((char *)value, NULL));
      break;
    default:
      break;
  }
  if (in_array)
    ++index;
}

void parseACKJson(const char *data)
{
  JsonStreamingParser parser;
  ParseACKJsonParser handler;
  parser.setListener(&handler);
  while (*data != 0)
  {
    parser.parse(*data++);
  }
}
