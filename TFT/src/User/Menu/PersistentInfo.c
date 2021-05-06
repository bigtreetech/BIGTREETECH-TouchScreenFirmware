#include "PersistentInfo.h"
#include "includes.h"

// check current menu to avoid display info
bool temperatureStatusValid(void)
{
  if (infoSettings.persistent_info != 1) return false;
  if (infoHost.connected == false) return false;
  if (toastRunning()) return false;

  if (infoMenu.menu[infoMenu.cur] == menuPrinting) return false;
  if (infoMenu.menu[infoMenu.cur] == menuStatus) return false;
  if (infoMenu.menu[infoMenu.cur] == menuMove) return false;
  if (infoMenu.menu[infoMenu.cur] == menuInfo) return false;
  if (infoMenu.menu[infoMenu.cur] == menuNotification) return false;

  return true;
}

// check temperature change
void loopTemperatureStatus(void)
{
  if (getMenuType() == MENU_TYPE_FULLSCREEN) return;
  if (!temperatureStatusValid()) return;

  uint8_t tmpHeater[3];  // chamber, bed, hotend
  uint8_t tmpIndex = 0;

  if (infoSettings.hotend_count)  // global hotend
    tmpHeater[tmpIndex++] = heatGetCurrentHotend();

  if (infoSettings.bed_en)  // global bed
    tmpHeater[tmpIndex++] = BED;

  if (infoSettings.chamber_en)  // global chamber
    tmpHeater[tmpIndex++] = CHAMBER;

  bool update = false;
  static int16_t lastCurrent[3];
  static int16_t lastTarget[3];

  for (int8_t i = tmpIndex - 1; i >= 0; i--)
  {
    int16_t actCurrent = heatGetCurrentTemp(tmpHeater[i]);
    int16_t actTarget = heatGetTargetTemp(tmpHeater[i]);

    if (lastCurrent[i] != actCurrent || lastTarget[i] != actTarget)
    {
      lastCurrent[i] = actCurrent;
      lastTarget[i] = actTarget;
      update = true;
    }
  }

  if (update) menuReDrawCurTitle();
}

// draw temperature status on title bar
int16_t drawTemperatureStatus(void)
{
  int16_t x_offset = LCD_WIDTH - 10;

  if (!temperatureStatusValid()) return x_offset;

  uint8_t tmpHeater[3];  // chamber, bed, 1-2hotend
  uint16_t tmpIcon[3];
  uint8_t tmpIndex = 0;

  if (infoSettings.hotend_count)
  {  // global hotend
    if (infoSettings.hotend_count == 2 && !infoSettings.chamber_en )  // dual hotend
    {
      tmpIcon[tmpIndex] = ICON_GLOBAL_NOZZLE;
      tmpHeater[tmpIndex++] = NOZZLE0;
      tmpIcon[tmpIndex] = ICON_GLOBAL_NOZZLE;
      tmpHeater[tmpIndex++] = NOZZLE1;
    }
    else  // single or mixing hotend
    {
      tmpIcon[tmpIndex] = ICON_GLOBAL_NOZZLE;
      tmpHeater[tmpIndex++] = heatGetCurrentHotend();
    }
  }

  if (infoSettings.bed_en)
  {  // global bed
    tmpIcon[tmpIndex] = ICON_GLOBAL_BED;
    tmpHeater[tmpIndex++] = BED;
  }

  if (infoSettings.chamber_en)
  {  // global chamber
    tmpIcon[tmpIndex] = ICON_GLOBAL_CHAMBER;
    tmpHeater[tmpIndex++] = CHAMBER;
  }

  uint16_t start_y = (TITLE_END_Y - BYTE_HEIGHT) / 2;

  GUI_SetBkColor(infoSettings.title_bg_color);

  for (int8_t i = tmpIndex - 1; i >= 0; i--)
  {
    char tempstr[10];

    x_offset -= GLOBALICON_INTERVAL;
    GUI_ClearRect(x_offset, start_y, x_offset + GLOBALICON_INTERVAL, start_y + GLOBALICON_HEIGHT);
    sprintf(tempstr, "%d/%d", heatGetCurrentTemp(tmpHeater[i]), heatGetTargetTemp(tmpHeater[i]));

    x_offset -= GUI_StrPixelWidth((uint8_t *)tempstr);
    GUI_StrPixelWidth(LABEL_10_PERCENT);

    GUI_DispString(x_offset, start_y, (uint8_t *)tempstr);  // value
    x_offset -= GLOBALICON_INTERVAL;
    GUI_ClearRect(x_offset, start_y, x_offset + GLOBALICON_INTERVAL, start_y + GLOBALICON_HEIGHT);
    x_offset -= GLOBALICON_WIDTH;
    ICON_ReadDisplay(x_offset, start_y, tmpIcon[i]);  // icon
  }

  GUI_SetBkColor(infoSettings.bg_color);

  return x_offset;
}
