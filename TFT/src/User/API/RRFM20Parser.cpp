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
    // if M20 S3 ever works, we can make use of this
    case SORT_DATE_NEW_FIRST:
      return ((M20_LIST_ITEM *)b)->timestamp - ((M20_LIST_ITEM *)a)->timestamp;
    case SORT_DATE_OLD_FIRST:
      return ((M20_LIST_ITEM *)a)->timestamp - ((M20_LIST_ITEM *)b)->timestamp;
    case SORT_NAME_ASCENDING:
      return strcasecmp(((M20_LIST_ITEM *)a)->file_name, ((M20_LIST_ITEM *)b)->file_name);
    case SORT_NAME_DESCENDING:
      return strcasecmp(((M20_LIST_ITEM *)b)->file_name, ((M20_LIST_ITEM *)a)->file_name);
  }
  return strcmp(((M20_LIST_ITEM *)a)->file_name, ((M20_LIST_ITEM *)b)->file_name);
}

void RRFM20Parser::startObject()
{
  in_object = in_files;
}

void RRFM20Parser::endObject()
{
  in_object = false;
  if (in_files && fileCount < FILE_NUM)
    ++fileCount;
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
        // TODO use this implicit sort until M20 S3 works
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
      free(fileList[i].display_name);
      fileList[i].display_name = NULL;
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
  state = none;
  if (!in_array)
    in_files = strcmp(FILES, key) == 0;

  if (in_files)
  {
    if (strcmp(FILES_TYPE, key) == 0)
      state = type;
    else if (strcmp(FILES_NAME, key) == 0)
      state = name;
    else if (strcmp(FILES_DATE, key) == 0)
      state = date;
  }
}

void RRFM20Parser::value(const char *value)
{
  if (!in_files)
    return;

  if (in_object)
  {
    if (fileCount >= FILE_NUM)
      return;

    uint16_t len = strlen(value + 1);

    switch (state)
    {
      case type:
        fileList[fileCount].is_directory = value[0] == 'd';
        break;

      case name:
      {
        if ((fileList[fileCount].file_name = (TCHAR *)malloc(len)) != NULL)
          strcpy(fileList[fileCount].file_name, value);
        const char *skipped = macro_sort ? skip_number(value) : value;
        len = strlen(skipped) + 1;
        if (macro_sort && value != skipped && (fileList[fileCount].display_name = (TCHAR *)malloc(len)) != NULL)
        {
          strcpy(fileList[fileCount].display_name, skipped);
        }
        else
        {
          fileList[fileCount].display_name = fileList[fileCount].file_name;
        }
        break;
      }

      case date:
      {
        // convert to uint to enable sorting: 2021-08-11T07:06:28
        char *out;
        uint8_t year = strtol(value,   &out, 10) - 1970; // will allow up to year 2225
        uint8_t mnth = strtol(out + 1, &out, 10);
        uint8_t date = strtol(out + 1, &out, 10);
        uint8_t hour = strtol(out + 1, &out, 10);
        uint8_t mins = strtol(out + 1, &out, 10);
        uint8_t secs = strtol(out + 1, NULL, 10);
        // uint32_t will allow about up until year 2098, 31 days in a month because I'm lazy
        fileList[fileCount].timestamp = secs + (mins * 60) + (hour * 60 * 60) +
          (date * 60 * 60 * 24) + (mnth * 31 * 60 * 60 * 24) + (year * 12 * 31 * 60 * 60 * 24);
        break;
      }

      case none:
        break;
    }
    state = none;
  }
  else
  {
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
}

void parseM20Response(const char *data, bool macro_sorting)
{
  static RRFM20Parser *handler = NULL;

  if (handler == NULL)
  {
    handler = new RRFM20Parser;
  }

  handler->macro_sort = macro_sorting;
  jsonStreamingParser.setListener(handler);
  while (*data != 0)
  {
    jsonStreamingParser.parse(*data++);
  }

  if (handler->need_reset)
  {
    jsonStreamingParser.reset();
    delete handler;
    handler = NULL;
  }
}

void parseMacroListResponse(const char *data)
{
  parseM20Response(data, true);
}

void parseJobListResponse(const char  *data)
{
  parseM20Response(data, false);
}
