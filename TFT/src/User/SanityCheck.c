#include "SanityCheck.h"

// Main settings
void sizecheck(void)
{
  // if error shows 'the size of an array must be greater than zero'
  // then the size of the array is larger than allocated size in flash.
  SIZE_CHECK(sizeof(SETTINGS) > PARA_SIZE);                          // Size of infoSettings is larger than allocated size in flash.
  SIZE_CHECK(sizeof(STRINGS_STORE) > STRINGS_STORE_MAX_SIZE);        // Size of strings_store is larger than allocated size in flash.
  SIZE_CHECK(sizeof(CUSTOM_GCODES) > CUSTOM_GCODE_MAX_SIZE);         // Size of custom_gcodes is larger than allocated size in flash.
  SIZE_CHECK(sizeof(PRINT_GCODES) > PRINT_GCODES_MAX_SIZE);          // Size of print_gcodes is larger than allocated size in flash.
  SIZE_CHECK((LABEL_NUM * MAX_LANG_LABEL_LENGTH) > LANGUAGE_SIZE);   // Size of Language pack is larger than allocated size in flash.
}
