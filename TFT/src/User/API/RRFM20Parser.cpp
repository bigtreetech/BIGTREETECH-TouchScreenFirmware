#include "RRFM20Parser.hpp"

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

int compare_items(const void *a, const void *b)
{
  return strcmp(((M20_LIST_ITEM *)a)->file_name, ((M20_LIST_ITEM *)b)->file_name);
}

void RRFM20Parser::endDocument()
{
  qsort(fileList, fileCount, sizeof(M20_LIST_ITEM), compare_items);

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
}

void RRFM20Parser::handle_value(const char *value)
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