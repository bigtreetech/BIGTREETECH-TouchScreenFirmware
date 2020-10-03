#define _SetLanguage_H_
// Please set LANGUAGE_CHECK and DEAGAULT_LANGUAGE here. 

/*
# Select the language to display on the LCD while in Touch Mode.
# Options: [ENGLISH: 0,  CHINESE: 1,     RUSSIAN: 2,  JAPANESE: 3,    ARMENIAN: 4,
#            GERMAN: 5,    CZECH: 6,       SPAIN: 7,    FRENCH: 8,  PORTUGUESE: 9,
#           ITALIAN: 10,  POLISH: 11,     SLOVAK: 12,    DUTCH: 13,  HUNGARIAN: 14,
#           TURKISH: 15,   GREEK: 16,  SLOVENIAN: 17,  CATALAN: 18]
*/
#ifdef LANGUAGE_CHECK
#undef LANGUAGE_CHECK
#define LANGUAGE_CHECK 0
#endif

/**
 * Default Touch Mode Language
 * Select the language to display on the LCD while in Touch Mode.
 * Options: ARMENIAN, CHINESE, CZECH, DUTCH, ENGLISH, FRENCH, GERMAN, HUNGARIAN, ITALIAN, JAPANESE, POLISH, PORTUGUESE, RUSSIAN, SLOVAK, SPAIN, CATALAN
 */
#ifdef DEFAULT_LANGUAGE
#undef DEFAULT_LANGUAGE
#define DEFAULT_LANGUAGE ENGLISH
#endif


/* 
*********************changes made to make language set at compile time **********
added #include "SetLanguage.h" to: language.h and configuration.h
edited default_language and its comment in configuration.h
Removed set language from screensettings menu. replaced with background.
*************************** can removed *********
* after some more tweaking the following maynot be needed:
* config.c (line 470):
     case C_INDEX_LANGUAGE:
    if (inLimit(config_int(), 0, LANGUAGE_NUM-1))
      infoSettings.language = config_int();
    break;
*
* config.h (line 20)
    #define CONFIG_LANGUAGE             "language:"
*
* Config.ini (line 37 to line 43)
*
*Settings.h line 78
*
*congif.inc line 12 X_CONFIG(LANGUAGE)
*
*flashstore.c line 75 and line 211
 infoSettings.language             = byteToWord(data + (index += 4), 4);
*
* language.h line 11 to line 35
*/