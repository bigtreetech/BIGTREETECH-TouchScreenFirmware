#include "RRFMacrosParser.hpp"

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
  return strcmp(((MACRO_LIST_ITEM *)a)->file_name, ((MACRO_LIST_ITEM *)b)->file_name);
}

void RRFMacrosParser::endDocument()
{
  qsort(macroList, macroCount, sizeof(MACRO_LIST_ITEM), compare_items);

  for (int i = 0; i < macroCount; i++)
  {
    if (macroList[i].is_directory)
    {
      infoFile.folder[infoFile.folderCount++] = macroList[i].file_name;
    }
    else
    {
      infoFile.Longfile[infoFile.fileCount] = macroList[i].file_name;
      infoFile.file[infoFile.fileCount++] = macroList[i].display_name;
    }
  }
}

void RRFMacrosParser::handle_value(const char *value)
{
  uint16_t current = macroCount++;
  if (current >= FILE_NUM)
    return;

  if ((macroList[current].is_directory = (*value == '*')))
  {
    ++value;
  }
  uint16_t len = strlen(value) + 1;

  if ((macroList[current].file_name = (TCHAR *)malloc(len)) != NULL)
    strcpy(macroList[current].file_name, value);

  value = skip_number(value);
  len = strlen(value) + 1;
  if ((macroList[current].display_name = (TCHAR *)malloc(len)) != NULL)
    strcpy(macroList[current].display_name, value);
}

void parseMacroListResponse(char *data)
{
  JsonStreamingParser parser;
  RRFMacrosParser handler;
  parser.setListener(&handler);
  while (*data != 0)
  {
    parser.parse(*data++);
  }
}