#ifndef _RRF_MACROS_PARSER_H_
#define _RRF_MACROS_PARSER_H_

#include <string.h>
#include <stdlib.h>
#include "ff.h"
#include "vfs.h"

#ifdef __cplusplus
extern "C"
{
#endif
  typedef struct
  {
    bool is_directory;
    TCHAR *display_name;
    TCHAR *file_name;
  } MACRO_LIST_ITEM;
  void parseMacroListResponse(char *data);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "JsonStreamingParser.hpp"
#include <string.h>

static const char *FILES = "files";
class RRFMacrosParser : public JsonListener
{

private:
  bool in_array = false;
  bool in_files = false;
  uint16_t macroCount = 0;
  MACRO_LIST_ITEM macroList[FILE_NUM];

public:
  inline void startDocument() {}
  inline void startObject() {}
  inline void endObject() {}
  inline void whitespace(char c) {}
  virtual void endDocument();
  inline void startArray()
  {
    in_array = in_files;
  }
  inline void endArray()
  {
    in_array = false;
  }

  inline void key(const char *key)
  {
    in_files = strcmp(FILES, key) == 0;
  }
  inline void value(const char *value)
  {
    if (in_array)
      handle_value(value);
  }
  void handle_value(const char *value);
};
#endif

#endif