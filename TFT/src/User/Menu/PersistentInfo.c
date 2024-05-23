#include "PersistentInfo.h"
#include "includes.h"

// global Info
#define GLOBALICON_WIDTH    (BYTE_WIDTH * 2)
#define GLOBALICON_HEIGHT   GLOBALICON_WIDTH
#define GLOBALICON_INTERVAL 2

// check current menu to avoid display info
static bool temperatureStatusValid(void)
{
  if (infoSettings.persistent_info != 1) return false;
  if (infoHost.connected == false) return false;
  if (toastRunning()) return false;

  if (MENU_IS(menuPrinting)) return false;
  if (MENU_IS(menuStatus)) return false;
  if (MENU_IS(menuMove)) return false;
  if (MENU_IS(menuInfo)) return false;
  if (MENU_IS(menuNotification)) return false;

  return true;
}

// check temperature change
void loopTemperatureStatus(void)
{
  if (getMenuType() == MENU_TYPE_FULLSCREEN) return;
  if (!temperatureStatusValid()) return;

  static int16_t lastCurrent[4];  // 1-2 hotend, bed, chamber
  static int16_t lastTarget[4];

  uint8_t tmpHeater[4];  // 1-2 hotend, bed, chamber
  uint8_t tmpIndex = 0;
  bool update = false;

  if (infoSettings.hotend_count)  // global hotend
  {
    if (infoSettings.hotend_count == 2 && !infoSettings.chamber_en)  // dual hotend
    {
      tmpHeater[tmpIndex++] = NOZZLE0;
      tmpHeater[tmpIndex++] = NOZZLE1;
    }
    else  // single or mixing hotend
    {
      tmpHeater[tmpIndex++] = heatGetCurrentHotend();
    }
  }

  if (infoSettings.bed_en)  // global bed
    tmpHeater[tmpIndex++] = BED;

  if (infoSettings.chamber_en)  // global chamber
    tmpHeater[tmpIndex++] = CHAMBER;

  while (tmpIndex > 0)
  {
    tmpIndex--;

    int16_t actCurrent = heatGetCurrentTemp(tmpHeater[tmpIndex]);
    int16_t actTarget = heatGetTargetTemp(tmpHeater[tmpIndex]);

    if (lastCurrent[tmpIndex] != actCurrent || lastTarget[tmpIndex] != actTarget)
    {
      lastCurrent[tmpIndex] = actCurrent;
      lastTarget[tmpIndex] = actTarget;

      update = true;
    }
  }

  if (update)
    menuDrawTitle();
}

// draw temperature status on title bar
int16_t drawTemperatureStatus(void)
{
  int16_t x_offset = LCD_WIDTH - 10;

  if (!temperatureStatusValid())
    return x_offset;

  uint16_t tmpIcon[4];  // 1-2 hotend, bed, chamber
  uint8_t tmpHeater[4];
  uint8_t tmpIndex = 0;

  if (infoSettings.hotend_count)
  { // global hotend
    if (infoSettings.hotend_count == 2 && !infoSettings.chamber_en)  // dual hotend
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
  { // global bed
    tmpIcon[tmpIndex] = ICON_GLOBAL_BED;
    tmpHeater[tmpIndex++] = BED;
  }

  if (infoSettings.chamber_en)
  { // global chamber
    tmpIcon[tmpIndex] = ICON_GLOBAL_CHAMBER;
    tmpHeater[tmpIndex++] = CHAMBER;
  }

  uint16_t start_y = (TITLE_END_Y - BYTE_HEIGHT) / 2;

  GUI_SetBkColor(infoSettings.title_bg_color);

  while (tmpIndex > 0)
  {
    tmpIndex--;

    char tempstr[10];

    x_offset -= GLOBALICON_INTERVAL;
    GUI_ClearRect(x_offset, start_y, x_offset + GLOBALICON_INTERVAL, start_y + GLOBALICON_HEIGHT);

    sprintf(tempstr, "%d/%d", heatGetCurrentTemp(tmpHeater[tmpIndex]), heatGetTargetTemp(tmpHeater[tmpIndex]));
    x_offset -= GUI_StrPixelWidth((uint8_t *)tempstr);
    GUI_DispString(x_offset, start_y, (uint8_t *)tempstr);  // value

    x_offset -= GLOBALICON_INTERVAL;
    GUI_ClearRect(x_offset, start_y, x_offset + GLOBALICON_INTERVAL, start_y + GLOBALICON_HEIGHT);

    x_offset -= GLOBALICON_WIDTH;
    ICON_ReadDisplay(x_offset, start_y, tmpIcon[tmpIndex]);  // icon
  }

  GUI_SetBkColor(infoSettings.bg_color);

  return x_offset;
}
