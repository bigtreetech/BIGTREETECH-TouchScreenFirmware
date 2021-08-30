#include "RRFM20Parser.hpp"
#include "FlashStore.h"

/*
  Parses a document like the following for M20 S2
  {
    "dir": "0:/gcodes/",
    "first": 0,
    "files": [
        "sample.gcode",
        ...
    ],
    "next": 0,
    "err": 0
  }

  Also parses a document like the following for M20 S3
  {
    "dir": "0:/gcodes/",
    "first": 0,
    "files": [
        {
            "type": "f",
            "name": "Phone Stand.gcode",
            "size": 2126889,
            "date": "2021-06-28T09:28:24"
        },
        {
            "type": "d",
            "name": "Test",
            "size": 0,
            "date": "2021-08-25T16:38:20"
        },
        ...
    ],
    "next": 0
  }
 */
const TCHAR *skip_number(const TCHAR *value)
{
  if (isdigit(*value))
  {
    do
    {
      ++value;
    } while (isdigit(*value));
    return (*value == '_') ? value + 1 : value;
  }

  return value;
}

int compare_items(void *arg, const void *a, const void *b)
{
  bool macro_sort = *(bool *)arg;
  if (macro_sort)
    return strcasecmp(((M20_LIST_ITEM *)a)->file_name, ((M20_LIST_ITEM *)b)->file_name);

  switch (infoSettings.files_sort_by)
  {
    case SORT_NAME_ASCENDING:
      return strcasecmp(((M20_LIST_ITEM *)a)->file_name, ((M20_LIST_ITEM *)b)->file_name);
    case SORT_NAME_DESCENDING:
      return strcasecmp(((M20_LIST_ITEM *)b)->file_name, ((M20_LIST_ITEM *)a)->file_name);
  }
  return strcmp(((M20_LIST_ITEM *)a)->file_name, ((M20_LIST_ITEM *)b)->file_name);
}

void RRFM20Parser::endDocument()
{
  if (macro_sort)
    qsort_r(fileList, fileCount, sizeof(M20_LIST_ITEM), &macro_sort, compare_items);
  else
  {
    switch (infoSettings.files_sort_by)
    {
      case SORT_DATE_NEW_FIRST:
        // M20 appears to be sorted oldest first, implicitly, reverse it.
        int i, j;
        for (i = 0, j = fileCount - 1; i < j; ++i, --j)
        {
          M20_LIST_ITEM tmp;
          tmp = fileList[i];
          fileList[i] = fileList[j];
          fileList[j] = tmp;
        }
        break;
      case SORT_NAME_ASCENDING:
      case SORT_NAME_DESCENDING:
          qsort_r(fileList, fileCount, sizeof(M20_LIST_ITEM), &macro_sort, compare_items);
        break;
    }
  }

  for (int i = 0; i < fileCount; i++)
  {
    if (fileList[i].is_directory)
    {
      infoFile.folder[infoFile.folderCount++] = fileList[i].file_name;
    }
    else
    {
      infoFile.Longfile[infoFile.fileCount] = fileList[i].file_name;
      infoFile.file[infoFile.fileCount++] = fileList[i].display_name;
    }

  }
  need_reset = true;
}

// TODO handle `next` parameter when there are too many files to list
// above ~8k response payloads result in pagination that needs handling
void RRFM20Parser::key(const char *key)
{
  if (!in_array)
    in_files = strcmp(FILES, key) == 0;
}

void RRFM20Parser::value(const char *value)
{
  if (!in_files)
    return;

  uint16_t current = fileCount++;
  if (current >= FILE_NUM)
    return;

  if ((fileList[current].is_directory = (*value == '*')))
  {
    ++value;
  }
  uint16_t len = strlen(value) + 1;

  if ((fileList[current].file_name = (TCHAR *)malloc(len)) != NULL)
    strcpy(fileList[current].file_name, value);

  value = macro_sort ? skip_number(value) : value;
  len = strlen(value) + 1;
  if ((fileList[current].display_name = (TCHAR *)malloc(len)) != NULL)
    strcpy(fileList[current].display_name, value);
}

void parseM20Response(const char *data, bool macro_sorting)
{
  JsonStreamingParser parser;
  RRFM20Parser handler;
  handler.macro_sort = macro_sorting;
  parser.setListener(&handler);
  while (*data != 0)
  {
    parser.parse(*data++);
  }
}

void parseMacroListResponse(const char *data)
{
  parseM20Response(data, true);
}

void parseJobListResponse(const char *data)
{
  parseM20Response(data, false);
}

void parseJobListResponseStreaming(const char  *data)
{
  static JsonStreamingParser parser;
  static RRFM20Parser handler;

  handler.macro_sort = false;
  parser.setListener(&handler);
  while (*data != 0)
  {
    parser.parse(*data++);
  }

  if (handler.need_reset)
  {
    handler.reset();
    parser.reset();
  }
}
