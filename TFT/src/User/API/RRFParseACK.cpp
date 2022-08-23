#include "RRFParseACK.hpp"
#include "includes.h"

/*
 * Parses structures like follows:
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
 *    "fraction_printed": 0,
 *    "msgBox.mode": -1
 *  }
 *
 * M291 messages extends the above with:
 * {
 *   ...
 *   "msgBox.mode": u8, // S
 *   "msgBox.seq": u32, // autoincrement ID
 *   "msgBox.timeout": fSeconds, // T
 *   "msgBox.controls": AxesBitmap(ignored),
 *   "msgBox.msg": "message payload", // P
 *   "msgBox.title": "title payload", // R
 *   ...
 * }
 */

static int8_t m291_mode = 0;
static bool show_m291 = false;
static uint32_t m291_seq = 0xffffffff;
static uint32_t expire_time = 0;

static void m291_confirm(void)
{
  if (m291_mode >= 1) mustStoreCmd("M292 P0\n");
  if (rrfStatusIsMacroBusy())
    rrfShowRunningMacro();
}

static void m291_cancel(void)
{
  if (m291_mode > 2) mustStoreCmd("M292 P1\n");
  if (m291_mode == 2) mustStoreCmd("M292 P0\n");
  if (rrfStatusIsMacroBusy())
    rrfShowRunningMacro();
}

static void m291_loop(void)
{
  if (m291_mode == -1 || (expire_time > 0 && OS_GetTimeMs() > expire_time))
  {
    CLOSE_MENU();
    if (rrfStatusIsMacroBusy())
      rrfShowRunningMacro();
  }
}

void ParseACKJsonParser::endDocument()
{
  requestCommandInfo.inJson = false;

  if (show_m291 && m291_msg != NULL)
  {
    char M291[] = "M291";
    // _Generic is not available in C++
    //setDialogText(m291_title, m291_msg, LABEL_CONFIRM, LABEL_CANCEL);
    _setDialogTitleStr((uint8_t *)(m291_title == NULL ? M291 : m291_title));
    _setDialogMsgStr((uint8_t *)m291_msg);
    _setDialogOkTextLabel(LABEL_CONFIRM);
    _setDialogCancelTextLabel(m291_mode > 2 ? LABEL_CANCEL : LABEL_NULL);
    expire_time = m291_timeo > 0 ? OS_GetTimeMs() + m291_timeo : 0;
    showDialog(m291_mode > 2 ? DIALOG_TYPE_QUESTION : DIALOG_TYPE_INFO, m291_confirm,
        m291_mode > 2 ? m291_cancel : NULL, m291_loop);
    BUZZER_PLAY(SOUND_NOTIFY);
    show_m291 = false;
  }

  if (m291_msg != NULL)
  {
    free(m291_msg);
    m291_msg = NULL;
  }

  if (m291_title != NULL)
  {
    free(m291_title);
    m291_title = NULL;
  }
  need_parser_reset = true;
}

void ParseACKJsonParser::value(const char *value)
{
  uint32_t seq;
  char *string_end;
  char *string_start;
  switch (state)
  {
    case status:
      rrfStatusSet(value[0]);
      break;
    case heaters:
      if (index == 0)
      {
        heatSetCurrentTemp(BED, strtod((char *)value, NULL) + 0.5f);
      }
      else if (index <= INVALID_HEATER)
      {
        heatSetCurrentTemp(index - 1, strtod((char *)value, NULL) + 0.5f);
      }
      break;
    case active:
      if (index == 0)
      {
        heatSetTargetTemp(BED, strtod((char *)value, NULL) + 0.5f, FROM_HOST);
      }
      else if (index <= INVALID_HEATER)
      {
        heatSetTargetTemp(index - 1, strtod((char *)value, NULL) + 0.5f, FROM_HOST);
      }
      break;
    case standby:
      break;
    case hstat:
      if (strtod((char *)value, NULL) == 3)
      {
        if (index == 0)
        {
          heatSetTargetTemp(BED, 0, FROM_HOST);
        }
        else if (index <= INVALID_HEATER)
        {
          heatSetTargetTemp(index - 1, 0, FROM_HOST);
        }
      }
      break;
    case pos:
      coordinateSetAxisActual((AXIS)index, strtod((char *)value, NULL));
      break;
    case sfactor:
      speedSetCurPercent(0, strtod((char *)value, NULL));
      break;
    case efactor:
      if (index == heatGetCurrentTool())
      {
        speedSetCurPercent(1, strtod((char *)value, NULL));
      }
      break;
    case baby_step:
      babystepSetValue(strtod((char *)value, NULL));
      break;
    case tool:
      break;
    case probe:
      break;
    case fan_percent:
      if (index != 0 && index <= infoSettings.fan_count) // index 0 is an alias for default tool fan
      {
        fanSetPercent(index - 1, strtod((char *)value, NULL) + 0.5f);
      }
      break;
    case fanRPM:
      break;
    case fraction_printed:
      if (getPrintProgressSource() < PROG_RRF)
        setPrintProgressSource(PROG_RRF);
      if (getPrintProgressSource() == PROG_RRF)
        setPrintProgressPercentage((value[0] - '0') * 100 + (value[2] - '0') * 10 + (value[3] - '0'));
      break;
    case mbox_seq:
      seq = strtod((char *)value, NULL);
      show_m291 = seq != m291_seq;
      m291_seq = seq;
      break;
    case mbox_mode:
      m291_mode = strtod((char *)value, NULL);
      break;
    case mbox_msg:
      m291_msg = (char*)malloc(strlen(value) + 1);
      strcpy(m291_msg, value);
      break;
    case mbox_title:
      m291_title = (char*)malloc(strlen(value) + 1);
      strcpy(m291_title, value);
      break;
    case mbox_timeo:
      m291_timeo = SEC_TO_MS(strtod((char *)value, NULL));
      break;
    case resp:
      if (strstr(value, (char *)"Steps/"))       //parse M92
      {
        if ((value = strstr(value, (char *)"X: ")) != NULL ) setParameter(P_STEPS_PER_MM, AXIS_INDEX_X,  atoi(value + 3));
        if ((value = strstr(value, (char *)"Y: ")) != NULL ) setParameter(P_STEPS_PER_MM, AXIS_INDEX_Y,  atoi(value + 3));
        if ((value = strstr(value, (char *)"Z: ")) != NULL ) setParameter(P_STEPS_PER_MM, AXIS_INDEX_Z,  atoi(value + 3));
        if ((value = strstr(value, (char *)"E: ")) != NULL ) setParameter(P_STEPS_PER_MM, AXIS_INDEX_E0, atoi(value + 3));
      }
      else if ((string_start = strstr(value, (char *)"RepRapFirmware")) != NULL)    // parse M115
      {
        setupMachine(FW_REPRAPFW);
        string_end = strstr(string_start, "ELECTRONICS");
        infoSetFirmwareName((uint8_t *)string_start, string_end-string_start);
      }
      else if ((string_start = strstr(value, (char *)"access point")) != NULL)    //parse M552
      {
        string_end = strstr(string_start, ",");
        string_start += 13;
        infoSetAccessPoint((uint8_t *)string_start,  string_end-string_start);

        if ((string_start = strstr(string_start, (char *)"IP address")) != NULL)
        {
          string_end = strstr(string_start, "\\n");
          string_start += 11;
          infoSetIPAddress((uint8_t *)string_start,  string_end-string_start);
        }
      }
      else if ((string_start = strstr(value, (char *)"printing byte")) != NULL)       // parse M27  {"seq":21,"resp":"SD printing byte 1226/5040433\n"}
      {
        string_end = strstr(string_start, (char *)"/");
        setPrintProgressData(atoi(string_start + 14), atoi(string_end + 1));
      }
      else if (strstr(value, (char *)"Auto tuning heater") && strstr(value, (char *)"completed"))
      {
        pidUpdateStatus(PID_SUCCESS);
      }
      else if (strstr(value, (char *)"Error: M303") || (strstr(value, (char *)"Auto tune of heater") && strstr(value, (char *)"failed")))
      {
        pidUpdateStatus(PID_FAILED);
      }

      break;
    case result:
        if (starting_print)
        {
          startRemotePrint(value);  // start print and open Printing menu
          starting_print = false;
        }
      break;
    case none:
      break;
  }
  if (in_array)
    ++index;
}

void rrfParseACK(const char *data)
{
  static ParseACKJsonParser handler;

  jsonStreamingParser.setListener(&handler);
  while (*data != 0)
  {
    jsonStreamingParser.parse(*data++);
  }
  if (handler.need_parser_reset)
  {
    jsonStreamingParser.reset();
    handler.need_parser_reset = false;
  }
}
