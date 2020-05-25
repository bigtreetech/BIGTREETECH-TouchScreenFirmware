#include "Numpad.h"

const GUI_RECT rect_of_numkey[KEY_NUM]={
  {0*SKEYWIDTH, ICON_START_Y+0*SKEYHEIGHT, 1*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT},//1
  {1*SKEYWIDTH, ICON_START_Y+0*SKEYHEIGHT, 2*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT},//2
  {2*SKEYWIDTH, ICON_START_Y+0*SKEYHEIGHT, 3*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT},//3
  {3*SKEYWIDTH, ICON_START_Y+0*SKEYHEIGHT, 4*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT},//OK

  {0*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT, 1*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT},//4
  {1*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT, 2*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT},//5
  {2*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT, 3*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT},//6
  {3*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT, 4*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT},//Del

  {0*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT, 1*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT},//7
  {1*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT, 2*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT},//8
  {2*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT, 3*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT},//9
  {3*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT, 4*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT},//

  {0*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT, 1*SKEYWIDTH, ICON_START_Y+4*SKEYHEIGHT},//.
  {1*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT, 2*SKEYWIDTH, ICON_START_Y+4*SKEYHEIGHT},//0
  {2*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT, 3*SKEYWIDTH, ICON_START_Y+4*SKEYHEIGHT},//-
  {3*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT, 4*SKEYWIDTH, ICON_START_Y+4*SKEYHEIGHT},//
};

const char *const numPadKeyChar[KEY_NUM] = {
  "1","2","3","\u0894",
  "4","5","6","\u0899",
  "7","8","9","\u0895",
  ".","0","-",""
};

//get keypress for NumPad
NUM_KEY_VALUES NumKeyGetValue(void)
{
  return (NUM_KEY_VALUES)KEY_GetValue(sizeof(rect_of_numkey)/sizeof(rect_of_numkey[0]), rect_of_numkey);
}

void Draw_keyboard(void)
{
    TSC_ReDrawIcon = NULL;
    GUI_ClearRect(0, 0, LCD_WIDTH, rect_of_numkey[0].y0);
    GUI_SetBkColor(WHITE);
    GUI_ClearRect(0, rect_of_numkey[0].y0, LCD_WIDTH, LCD_HEIGHT);
    GUI_SetColor(BLACK);

    for (int i = 0;i<3;i++){
        GUI_DrawLine(rect_of_numkey[i].x1,rect_of_numkey[i].y0,rect_of_numkey[12+i].x1,rect_of_numkey[12+i].y1);
        GUI_DrawLine(rect_of_numkey[i*4].x0,rect_of_numkey[i*4].y1,rect_of_numkey[3+i*4].x1,rect_of_numkey[3+i*4].y1);
    }

    for(uint8_t i=0 ;i<KEY_NUM ;i++)
    {

        //GUI_DrawPrect(&rect_of_numkey[i]);
        GUI_SetTextMode(GUI_TEXTMODE_TRANS);
        GUI_SetColor(BLACK);
        GUI_DispStringInPrect(&rect_of_numkey[i],(u8*)numPadKeyChar[i]);
        GUI_SetTextMode(GUI_TEXTMODE_NORMAL);

    }
    GUI_RestoreColorDefault();
}


float numPadFloat(float old_val, bool negative_val)
{
    //bool exit = false;
    GUI_RECT oldParameterRect = {0, 0, LCD_WIDTH/2 - BYTE_WIDTH, rect_of_numkey[0].y0};
    GUI_RECT newParameterRect = {LCD_WIDTH/2 + BYTE_WIDTH, 0, LCD_WIDTH, rect_of_numkey[0].y0};
    GUI_RECT arrowRect = {LCD_WIDTH/2 - BYTE_WIDTH, 0, LCD_WIDTH/2 + BYTE_WIDTH, rect_of_numkey[0].y0};
    uint8_t nowIndex = 0,lastIndex = 0;
    char ParameterBuf[BUFLONG] = {0};
    NUM_KEY_VALUES key_num = NUM_KEY_IDLE;
    //my_sprintf(ParameterBuf,"%f",old_val);
    Draw_keyboard();
    GUI_DispStringInPrect(&arrowRect,(u8 *)">");
    char tempstr[BUFLONG];
    my_sprintf(tempstr, "%.2f", old_val);
    //GUI_SetColor(WHITE);
    GUI_DispStringInPrect(&oldParameterRect,(u8*)tempstr);
    touchSound = false;
    while (1)
    {
      key_num = NumKeyGetValue();
      switch (key_num)
      {
      case NUM_KEY_EXIT:
        BUZZER_PLAY(sound_cancel);
        touchSound = true;
        return old_val;

      case NUM_KEY_DEL:
        if (nowIndex)
          ParameterBuf[--nowIndex] = 0;
          BUZZER_PLAY(sound_keypress);
        break;

      case NUM_KEY_1:
      case NUM_KEY_2:
      case NUM_KEY_3:
      case NUM_KEY_4:
      case NUM_KEY_5:
      case NUM_KEY_6:
      case NUM_KEY_7:
      case NUM_KEY_8:
      case NUM_KEY_9:
      case NUM_KEY_0:
        if (nowIndex < BUFLONG - 1)
        {
          ParameterBuf[nowIndex++] = numPadKeyChar[key_num][0];
          ParameterBuf[nowIndex] = 0;
          BUZZER_PLAY(sound_keypress);
        }
        break;
      case NUM_KEY_DEC:
        if (!strchr((const char *)ParameterBuf, numPadKeyChar[key_num][0]))
        {
          if (nowIndex < BUFLONG - 1)
          {
            if(nowIndex == 0 || (nowIndex == 1 && strchr((const char *)ParameterBuf, '-'))) // check if no number exits or only minus exists
              ParameterBuf[nowIndex++] = '0'; //add zero before decimal sign if it is the first character
            ParameterBuf[nowIndex++] = numPadKeyChar[key_num][0];
            ParameterBuf[nowIndex] = 0;
            BUZZER_PLAY(sound_keypress);
          }
        }
        break;
      case NUM_KEY_MINUS:
        if (!strchr( (const char *)ParameterBuf, numPadKeyChar[key_num][0]) && negative_val)
        {
          if (nowIndex == 0)
          {
            ParameterBuf[nowIndex++] = numPadKeyChar[key_num][0];
            ParameterBuf[nowIndex] = 0;
            BUZZER_PLAY(sound_keypress);
          }
        }
        break;

      case NUM_KEY_OK:
        if (nowIndex > 0)
        {
          if (nowIndex == 1 &&(strchr((const char *)ParameterBuf, '.') || strchr((const char *)ParameterBuf, '-')))
            break;
        //Send_Settingcmd();

        BUZZER_PLAY(sound_ok);
        touchSound = true;
        return strtof(ParameterBuf, NULL);

        }
      default:
        break;
      }

      if (lastIndex != nowIndex)
      {
        lastIndex = nowIndex;
        GUI_ClearPrect(&newParameterRect);
        GUI_DispStringInPrect(&newParameterRect, (u8 *)ParameterBuf);
      }
      loopBackEnd();
    }
}


u32 numPadInt(u32 old_val)
{
    //bool exit = false;
    GUI_RECT oldParameterRect = {0, 0, LCD_WIDTH/2 - BYTE_WIDTH, rect_of_numkey[0].y0};
    GUI_RECT newParameterRect = {LCD_WIDTH/2 + BYTE_WIDTH, 0, LCD_WIDTH, rect_of_numkey[0].y0};
    GUI_RECT arrowRect = {LCD_WIDTH/2 - BYTE_WIDTH, 0, LCD_WIDTH/2 + BYTE_WIDTH, rect_of_numkey[0].y0};
    uint8_t nowIndex = 0,lastIndex = 0;
    char ParameterBuf[BUFLONG] = {0};
    NUM_KEY_VALUES key_num = NUM_KEY_IDLE;
    Draw_keyboard();
    GUI_DispStringInPrect(&arrowRect,(u8 *)">");
    char tempstr[BUFLONG];
    my_sprintf(tempstr, "%d", old_val);
    GUI_DispStringInPrect(&oldParameterRect,(u8*)tempstr);
    touchSound = false;
    while (1)
    {
      key_num = NumKeyGetValue();
      switch (key_num)
      {
      case NUM_KEY_EXIT:
        BUZZER_PLAY(sound_cancel);
        touchSound = true;
        return old_val;

      case NUM_KEY_DEL:
        if (nowIndex)
          ParameterBuf[--nowIndex] = 0;
          BUZZER_PLAY(sound_keypress);
        break;

      case NUM_KEY_1:
      case NUM_KEY_2:
      case NUM_KEY_3:
      case NUM_KEY_4:
      case NUM_KEY_5:
      case NUM_KEY_6:
      case NUM_KEY_7:
      case NUM_KEY_8:
      case NUM_KEY_9:
      case NUM_KEY_0:
        if (nowIndex < BUFLONG - 1)
        {
          ParameterBuf[nowIndex++] = numPadKeyChar[key_num][0];
          ParameterBuf[nowIndex] = 0;
          BUZZER_PLAY(sound_keypress);
        }
        break;
      case NUM_KEY_OK:
        if (nowIndex > 0)
        {
          if (nowIndex == 1 &&(ParameterBuf[0] == '.' || ParameterBuf[0] == '-'))
            break;

        BUZZER_PLAY(sound_ok);
        touchSound = true;
        return strtoul(ParameterBuf,NULL,10);
        }
      default:
        break;
      }

      if (lastIndex != nowIndex)
      {
        lastIndex = nowIndex;
        GUI_ClearPrect(&newParameterRect);
        GUI_DispStringInPrect(&newParameterRect, (u8 *)ParameterBuf);
      }
      loopBackEnd();
    }
}
