#ifndef _RRF_M20_PARSER_H_
#define _RRF_M20_PARSER_H_

#include <string.h>
#include <stdlib.h>
#include "ff.h"
#include "gcode.h"
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
    uint32_t timestamp;
  } M20_LIST_ITEM;
  void parseJobListResponse(const char *data);
  void parseMacroListResponse(const char *data);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "JsonStreamingParser.hpp"
#include <string.h>

#define FILES "files"
#define FILES_TYPE "type"
#define FILES_NAME "name"
#define FILES_DATE "date"
enum RRFM20ParserState { none, type, name, date };

class RRFM20Parser : public JsonListener
{

private:
  bool in_array = false;
  bool in_object = false;
  bool in_files = false;
  uint16_t fileCount = 0;
  RRFM20ParserState state = none;
  M20_LIST_ITEM fileList[FILE_NUM];

public:
  bool macro_sort = false;
  bool need_reset = false;

  virtual ~RRFM20Parser() {}
  inline void startDocument() {}
  virtual void startObject();
  virtual void endObject();
  inline void whitespace(char c) {}
  virtual void endDocument();
  virtual void key(const char *key);
  virtual void value(const char *value);

  inline void reset()
  {
    in_array = false;
    in_object = false;
    in_files = false;
    fileCount = 0;
    state = none;
    need_reset = false;
  }

  inline void startArray()
  {
    in_array = in_files;
  }
  inline void endArray()
  {
    in_array = false;
    in_files = false;
  }

};
#endif

#endif