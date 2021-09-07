#ifndef _PARSE_ACK_JSON_H_
#define _PARSE_ACK_JSON_H_

#ifdef __cplusplus
extern "C"
{
#endif
  void rrfParseACK(const char *data);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "JsonStreamingParser.hpp"
#include <string.h>

#define FANS "fanPercent"
#define SPEED "sfactor"
#define EXTRUSION "efactor"
#define STATUS "status"
#define MBOX_SEQ "msgBox.seq"
#define MBOX_MODE "msgBox.mode"
#define MBOX_TIMEO "msgBox.timeout"
#define MBOX_MSG "msgBox.msg"
#define MBOX_TITLE "msgBox.title"

enum DOCUMENT_STATE { none, fan_percent, sfactor, efactor, status, mbox_seq, mbox_mode, mbox_timeo, mbox_msg, mbox_title };
class ParseACKJsonParser : public JsonListener
{

private:
  DOCUMENT_STATE state = none;
  bool in_array = false;

  uint16_t index = 0;

  char *m291_msg = NULL;
  char *m291_title = NULL;
  uint32_t m291_timeo = 0;

public:
  bool need_parser_reset = false;
  inline void startDocument() {}
  inline void startObject() {}
  inline void endObject() {}
  inline void whitespace(char c) {}
  virtual void endDocument();
  inline void startArray()
  {
    in_array = true;
    index = 0;
  }
  inline void endArray()
  {
    in_array = false;
  }

  inline void key(const char *key)
  {
    if (strcmp(FANS, key) == 0)
    {
      state = fan_percent;
    }
    else if (strcmp(SPEED, key) == 0)
    {
      state = sfactor;
    }
    else if (strcmp(EXTRUSION, key) == 0)
    {
      state = efactor;
    }
    else if (strcmp(STATUS, key) == 0)
    {
      state = status;
    }
    else if (strcmp(MBOX_SEQ, key) == 0)
    {
      state = mbox_seq;
    }
    else if (strcmp(MBOX_MODE, key) == 0)
    {
      state = mbox_mode;
    }
    else if (strcmp(MBOX_TITLE, key) == 0)
    {
      state = mbox_title;
    }
    else if (strcmp(MBOX_MSG, key) == 0)
    {
      state = mbox_msg;
    }
    else if (strcmp(MBOX_TIMEO, key) == 0)
    {
      state = mbox_timeo;
    }
    else
    {
      state = none;
    }
  }
  virtual void value(const char *value);
};
#endif

#endif
