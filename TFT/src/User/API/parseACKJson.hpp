#ifndef _PARSE_ACK_JSON_H_
#define _PARSE_ACK_JSON_H_

#ifdef __cplusplus
extern "C"
{
#endif
  void parseACKJson(const char *data);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "JsonStreamingParser.hpp"
#include <string.h>

static const char *FANS = "fanPercent";
static const char *SPEED = "sfactor";
static const char *EXTRUSION = "efactor";
enum DOCUMENT_STATE { none, fan_percent, sfactor, efactor };
class ParseACKJsonParser : public JsonListener
{

private:
  bool in_array = false;
  uint16_t index = 0;
  DOCUMENT_STATE state = none;

public:
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
    else
    {
      state = none;
    }
  }
  virtual void value(const char *value);
};
#endif

#endif
