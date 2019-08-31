#ifndef _LANGUAGE_AM_H_
#define _LANGUAGE_AM_H_

    // 0xA140 - Ա   // 0xA153 - Մ   // 0xA166 - ա   // 0xA179 - մ   // 0xA18c - ֆ
    // 0xA141 - Բ   // 0xA154 - Յ   // 0xA167 - բ   // 0xA17a - յ
    // 0xA142 - Գ   // 0xA155 - Ն   // 0xA168 - գ   // 0xA17b - ն
    // 0xA143 - Դ   // 0xA156 - Շ   // 0xA169 - դ   // 0xA17c - շ
    // 0xA144 - Ե   // 0xA157 - Ո   // 0xA16a - ե   // 0xA17d - ո
    // 0xA145 - Զ   // 0xA158 - Չ   // 0xA16b - զ   // 0xA17e - չ
    // 0xA146 - Է   // 0xA159 - Պ   // 0xA16c - է   // 0xA17f - պ
    // 0xA147 - Ը   // 0xA15a - Ջ   // 0xA16d - ը   // 0xA180 - ջ
    // 0xA148 - Թ   // 0xA15b - Ռ   // 0xA16e - թ   // 0xA181 - ռ
    // 0xA149 - Ժ   // 0xA15c - Ս   // 0xA16f - ժ   // 0xA182 - ս
    // 0xA14a - Ի   // 0xA15d - Վ   // 0xA170 - ի   // 0xA183 - վ
    // 0xA14b - Լ   // 0xA15e - Տ   // 0xA171 - լ   // 0xA184 - տ
    // 0xA14c - Խ   // 0xA15f - Ր   // 0xA172 - խ   // 0xA185 - ր
    // 0xA14d - Ծ   // 0xA160 - Ց   // 0xA173 - ծ   // 0xA186 - ց
    // 0xA14e - Կ   // 0xA161 - Ւ   // 0xA174 - կ   // 0xA187 - ու
    // 0xA14f - Հ   // 0xA162 - Փ   // 0xA175 - հ   // 0xA188 - փ
    // 0xA150 - Ձ   // 0xA163 - Ք   // 0xA176 - ձ   // 0xA189 - ք
    // 0xA151 - Ղ   // 0xA164 - Օ   // 0xA177 - ղ   // 0xA18a - և
    // 0xA152 - Ճ   // 0xA165 - Ֆ   // 0xA178 - ճ   // 0xA18b - օ

    #define AM_LANGUAGE             (u8[][2]){{0xA1, 0x4F}, {0xA1, 0x66}, {0xA1, 0x7A}, {0xA1, 0x6A}, {0xA1, 0x85}, {0xA1, 0x6A}, {0xA1, 0x7B}, '\0'}
    #define AM_HEAT                 (u8[][2]){{0xA1, 0x5A}, {0xA1, 0x6A}, {0xA1, 0x85}, {0xA1, 0x79}, {0xA1, 0x87}, {0xA1, 0x6e}, {0xA1, 0x7a}, {0xA1, 0x87}, {0xA1, 0x7b}, '\0'}
    #define AM_MOVE                 (u8[][2]){{0xA1, 0x5E}, {0xA1, 0x6A}, {0xA1, 0x77}, {0xA1, 0x66}, {0xA1, 0x88}, {0xA1, 0x7d}, {0xA1, 0x72}, {0xA1, 0x6A}, {0xA1, 0x71}, '\0'}
    #define AM_HOME                 (u8[][2]){{0xA1, 0x5C}, {0xA1, 0x70}, {0xA1, 0x6B}, {0xA1, 0x67}, '\0'}
    #define AM_PRINT                (u8[][2]){{0xA1, 0x5E}, {0xA1, 0x7F}, {0xA1, 0x6A}, {0xA1, 0x71}, '\0'}
    #define AM_EXTRUDE              (u8[][2]){{0xA1, 0x43}, {0xA1, 0x87}, {0xA1, 0x85}, {0xA1, 0x82}, {0xA1, 0x8D}, {0xA1, 0x79}, {0xA1, 0x77}, {0xA1, 0x6A}, {0xA1, 0x71}, '\0'}
    #define AM_FAN                  (u8[][2]){{0xA1, 0x64}, {0xA1, 0x69}, {0xA1, 0x66}, {0xA1, 0x88}, {0xA1, 0x7D}, {0xA1, 0x72}, {0xA1, 0x70}, {0xA1, 0x7E}, '\0'}
    #define AM_SETTINGS             (u8[][2]){{0xA1, 0x4E}, {0xA1, 0x66}, {0xA1, 0x85}, {0xA1, 0x68}, {0xA1, 0x66}, {0xA1, 0x83}, {0xA1, 0x7D}, {0xA1, 0x85}, {0xA1, 0x87}, {0xA1, 0x79}, '\0'}
    #define AM_LEVELING             (u8[][2]){{0xA1, 0x4F}, {0xA1, 0x66}, {0xA1, 0x85}, {0xA1, 0x6E}, {0xA1, 0x6A}, {0xA1, 0x86}, {0xA1, 0x87}, {0xA1, 0x79}, '\0'}
    #define AM_INC                  "Increase"
    #define AM_DEC                  "Decrease"
    #define AM_NOZZLE               "Nozzle"
    #define AM_BED                  "Hotbed"
    #define AM_STOP                 "Stop"
    #define AM_BACK                 "Back"
    #define AM_PAGE_UP              "Page up"
    #define AM_PAGE_DOWN            "Page down"
    #define AM_PAUSE                "Pause"
    #define AM_RESUME               "Resume"
    #define AM_LOAD                 "Load"
    #define AM_UNLOAD               "Unload"
    #define AM_SLOW_SPEED           "Slow"
    #define AM_NORMAL_SPEED         "Normal"
    #define AM_FAST_SPEED           "Fast"
    #define AM_FAN_FULL_SPEED       "Full"
    #define AM_FAN_HALF_SPEED       "Half"
    #define AM_POWER_OFF            "Power Off"
    #define AM_TOUCHSCREEN_ADJUST   "TS Adjust"
    #define AM_SCREEN_INFO          "Info"
    #define AM_DISCONNECT           "Disconnect"
    #define AM_BAUDRATE_115200      "115200"
    #define AM_BAUDRATE_250000      "250000"
    #define AM_RUNOUT_OFF           "Runout OFF"
    #define AM_RUNOUT_ON            "Runout ON"
    #define AM_PERCENTAGE           "Percentage"
    #define AM_BABYSTEP             "Baby step"
    #define AM_PERCENTAGE_SPEED     "Speed"
    #define AM_PERCENTAGE_FLOW      "Flow"
    #define AM_VALUE_ZERO           "Zero"
    #define AM_1_DEGREE             "1��"
    #define AM_5_DEGREE             "5��"
    #define AM_10_DEGREE            "10��"
    #define AM_X_INC                "X+"
    #define AM_Y_INC                "Y+"
    #define AM_Z_INC                "Z+"
    #define AM_X_DEC                "X-"
    #define AM_Y_DEC                "Y-"
    #define AM_Z_DEC                "Z-"
    #define AM_X_HOME               "X"
    #define AM_Y_HOME               "Y"
    #define AM_Z_HOME               "Z"
    #define AM_001_MM               "0.01mm"
    #define AM_01_MM                "0.1mm"
    #define AM_1_MM                 "1mm"
    #define AM_5_MM                 "5mm"
    #define AM_10_MM                "10mm"
    #define AM_1_PERCENT            "1%"
    #define AM_5_PERCENT            "5%"
    #define AM_10_PERCENT           "10%"

    #define AM_READY                (u8[][2]){{0xA1, 0x59}, {0xA1, 0x66}, {0xA1, 0x84}, {0xA1, 0x85}, {0xA1, 0x66}, {0xA1, 0x82}, {0xA1, 0x84}, {0xA1, 0x8D}, {0xA1, 0x6C}, '\0'}
    #define AM_PRINTING             "Printing"
    #define AM_BUSY                 "Busy processing ,please wait..."
    #define AM_UNCONNECTED          "No printer attached!"
    #define AM_DISCONNECT_INFO      "Now you can control the printer by your computer!"
    #define AM_LOADING              "Loading..."
    #define AM_READ_SD_ERROR        "Read SD card error!"    
    #define AM_POWER_FAILED         "Continue printing?"
    #define AM_CONTINUE             "Continue"
    #define AM_CANNEL               "Cancel"
    #define AM_ADJUST_TITLE         "Touch Screen Calibration"
    #define AM_ADJUST_INFO          "Please click on the red dot"
    #define AM_ADJUST_OK            "Adjustment Success"
    #define AM_ADJUST_FAILED        "Adjustment failed, Please Try Again"
    #define AM_WARNING              "Warning"
    #define AM_STOP_PRINT           "Stop printing?"
    #define AM_CONFIRM              "Confirm"

    #define AM_TFTSD                "TFT SD"
    #define AM_READ_TFTSD_ERROR     "Read TFT SD card error!"
    #define AM_TFTSD_INSERTED       "Card inserted!"
    #define AM_TFTSD_REMOVED        "Card removed!"
    
    #define AM_U_DISK               "U Disk"
    #define AM_READ_U_DISK_ERROR    "Read U Disk error!"
    #define AM_U_DISK_INSERTED      "U Disk inserted!"
    #define AM_U_DISK_REMOVED       "U Disk removed!"
    
    #define AM_ONBOARDSD            "OnboardSD"
    #define AM_READ_ONBOARDSD_ERROR "Read Onboard SD card error!"
    
    #define AM_FILAMENT_RUNOUT      "filament runout!"

#endif
