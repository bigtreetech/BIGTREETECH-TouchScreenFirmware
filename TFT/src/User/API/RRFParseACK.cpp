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
static bool need_parser_reset = false;

static JsonStreamingParser parser;
static ParseACKJsonParser handler;

static void m291_confirm(void)
{
  if (m291_mode > 1) mustStoreCmd("M292 P0\n");
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
    infoMenu.cur--;
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
    _setDialogCancelTextLabel(m291_mode > 2 ? LABEL_CANCEL : LABEL_BACKGROUND);
    expire_time = m291_timeo > 0 ? OS_GetTimeMs() + m291_timeo : 0;
    showDialog(m291_mode > 2 ? DIALOG_TYPE_QUESTION : DIALOG_TYPE_INFO, m291_confirm,
        m291_mode > 2 ? m291_cancel : NULL, m291_loop);
    BUZZER_PLAY(sound_notify);
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
    case status:
      rrfStatusSet(value[0]);
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
      m291_timeo = strtod((char *)value, NULL) * 1000;
      break;
    case none:
      break;
  }
  if (in_array)
    ++index;
}

void rrfParseACK(const char *data)
{
  parser.setListener(&handler);
  if (need_parser_reset)
  {
    parser.reset();
    need_parser_reset = false;
  }
  while (*data != 0)
  {
    parser.parse(*data++);
  }
}
