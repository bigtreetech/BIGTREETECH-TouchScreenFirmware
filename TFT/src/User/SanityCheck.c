#include "SanityCheck.h"

// main settings
void sizecheck(void)
{
  // if error shows 'the size of an array must be greater than zero' or 'size of unnamed array is negative'
  // then the size of the array is larger than allocated size in flash
  SIZE_CHECK((sizeof(SETTINGS) + 12 + sizeof(TS_CalPara)) > PARA_SIZE);  // size of infoSettings is larger than allocated size in flash
  SIZE_CHECK(sizeof(STRINGS_STORE) > STRINGS_STORE_MAX_SIZE);            // size of strings_store is larger than allocated size in flash
  SIZE_CHECK(sizeof(PREHEAT_STORE) > PREHEAT_STORE_MAX_SIZE);            // size of preheat_store is larger than allocated size in flash
  SIZE_CHECK(sizeof(CUSTOM_GCODES) > CUSTOM_GCODE_MAX_SIZE);             // size of custom_gcodes is larger than allocated size in flash
  SIZE_CHECK(sizeof(PRINT_GCODES) > PRINT_GCODES_MAX_SIZE);              // size of print_gcodes is larger than allocated size in flash
  SIZE_CHECK((LABEL_NUM * MAX_LANG_LABEL_LENGTH) > LANGUAGE_SIZE);       // size of Language pack is larger than allocated size in flash
}
