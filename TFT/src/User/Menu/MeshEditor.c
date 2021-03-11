#include "MeshEditor.h"
#include "includes.h"

#define MESH_GRID_SIZE (MESH_GRID_MAX_POINTS_X * MESH_GRID_MAX_POINTS_Y)
#define MESH_MAX_PARSED_ROWS 30                            // Set the maximum number of data rows to parse for retrieving the mesh
                                                           // grid from the "M420 T1 V1" command output provided by the Marlin FW
#define MESH_LINE_EDGE_DISTANCE 4

// colors
#define VALUE_FONT_COLOR   infoSettings.font_color
#define VALUE_BG_COLOR     infoSettings.list_button_color
#define VALUE_BORDER_COLOR 0x4b0d

typedef struct
{
  const uint8_t colsToSkip;
  const uint8_t rowsToSkip;
  const bool rowsInverted;
  const char *const echoMsg;
} MESH_DATA_FORMAT;

typedef enum
{
  ME_DATA_IDLE = 0,
  ME_DATA_EMPTY,
  ME_DATA_WAITING,
  ME_DATA_FULL,
  ME_DATA_FAILED,
} MESH_DATA_STATUS;

typedef struct
{
  float dataOrig[MESH_GRID_SIZE];                          // data grid of original Z height
  float data[MESH_GRID_SIZE];                              // data grid of current Z height data grid

  uint16_t dataSize;                                       // number of data present in the data grid
  uint16_t colsNum;                                        // number of points per X axis (number of columns in the data grid)
  uint16_t rowsNum;                                        // number of points per Y axis (number of rows in the data grid)

  uint16_t index;                                          // current index in the data grid
  uint16_t col;                                            // current column in the data grid
  uint16_t row;                                            // current row in the data grid

  bool initValueMinMax;                                    // force to init valueMin and valueMax
  float valueMin;                                          // minimum data value in the data grid
  float valueMax;                                          // maximum data value in the data grid
  float valueDelta;                                        // valueMax - valueMin

  uint16_t rStart;                                         // RGB red component for drawing the minimum data value in the data grid
  uint16_t gStart;                                         // RGB green component for drawing the minimum data value in the data grid
  uint16_t bStart;                                         // RGB blue component for drawing the minimum data value in the data grid
  uint16_t rEnd;                                           // RGB green component for drawing the maximum data value in the data grid
  uint16_t gEnd;                                           // RGB red component for drawing the maximum data value in the data grid
  uint16_t bEnd;                                           // RGB blue component for drawing the maximum data value in the data grid
  float rDelta;                                            // rEnd - rStart
  float gDelta;                                            // gEnd - gStart
  float bDelta;                                            // bEnd - bStart

  MESH_DATA_STATUS status;                                 // current status of dataOrig/data

  uint16_t parsedRows;

  uint16_t colsToSkip;
  uint16_t rowsToSkip;
  bool rowsInverted;

  char saveTitle[120];
} MESH_DATA;

#define MESH_GRID_HEIGHT     (LCD_HEIGHT - ICON_START_Y)
#define MESH_GRID_WIDTH      MESH_GRID_HEIGHT
#define MESH_POINT_MIN_RATIO 3.0f
#define MESH_POINT_MED_RATIO 5.0f
#define MESH_POINT_MAX_RATIO 8.0f

#define MESH_INFO_ROW_NUM 1
#define MESH_INFO_COL_NUM 3
#define MESH_INFO_HEIGHT  (ICON_START_Y / MESH_INFO_ROW_NUM)
#define MESH_INFO_WIDTH   (MESH_GRID_WIDTH / MESH_INFO_COL_NUM)

#define MESH_KEY_ROW_NUM 6
#define MESH_KEY_COL_NUM 2
#define MESH_KEY_HEIGHT  (LCD_HEIGHT / MESH_KEY_ROW_NUM)
#define MESH_KEY_WIDTH   (LCD_WIDTH - MESH_GRID_WIDTH) / MESH_KEY_COL_NUM

#ifdef MESH_LEFT_KEYBOARD
  #define MESH_GRID_X0 (LCD_WIDTH - MESH_GRID_WIDTH)
  #define MESH_GRID_Y0 ICON_START_Y
  #define MESH_INFO_X0 (LCD_WIDTH - MESH_GRID_WIDTH)
  #define MESH_INFO_Y0 0
  #define MESH_KEY_X0  0
  #define MESH_KEY_Y0  0
#else
  #define MESH_GRID_X0 0
  #define MESH_GRID_Y0 ICON_START_Y
  #define MESH_INFO_X0 0
  #define MESH_INFO_Y0 0
  #define MESH_KEY_X0  MESH_GRID_WIDTH
  #define MESH_KEY_Y0  0
#endif

typedef enum
{
  ME_INFO_MIN = 0,
  ME_INFO_MAX,
  ME_INFO_ORIG,
  ME_INFO_CUR,
  ME_INFO_NUM,                                             // number of infos
  ME_INFO_IDLE = IDLE_TOUCH,
} MESH_INFO_VALUES;

typedef enum
{
  ME_KEY_SAVE = 0,
  ME_KEY_OK,
  ME_KEY_RESET,
  ME_KEY_HOME,
  ME_KEY_EDIT,
  ME_KEY_UP,
  ME_KEY_PREV,
  ME_KEY_NEXT,
  ME_KEY_DOWN,
  ME_KEY_NUM,                                              // number of keys
  ME_KEY_IDLE = IDLE_TOUCH,
} MESH_KEY_VALUES;

typedef enum
{
  ME_AREA_GRID = 0,
  ME_AREA_INFO,
  ME_AREA_KEY,
  ME_AREA_NUM,                                             // number of areas
  ME_AREA_IDLE = IDLE_TOUCH,
} MESH_AREA_VALUES;

const GUI_RECT meshGridRect = {MESH_GRID_X0, MESH_GRID_Y0, MESH_GRID_X0 + MESH_GRID_WIDTH, MESH_GRID_Y0 + MESH_GRID_HEIGHT};

const GUI_RECT meshInfoRect[ME_INFO_NUM] = {
#ifdef MESH_LEFT_KEYBOARD
  {MESH_INFO_X0 + 1 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 0 * MESH_INFO_HEIGHT, MESH_INFO_X0 + 2 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 1 * MESH_INFO_HEIGHT},// min value
  {MESH_INFO_X0 + 2 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 0 * MESH_INFO_HEIGHT, MESH_INFO_X0 + 3 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 1 * MESH_INFO_HEIGHT},// max value
  {MESH_INFO_X0 + 0 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 0 * MESH_INFO_HEIGHT, MESH_INFO_X0 + 1 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 1 * MESH_INFO_HEIGHT},// original value
#else
  {MESH_INFO_X0 + 0 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 0 * MESH_INFO_HEIGHT, MESH_INFO_X0 + 1 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 1 * MESH_INFO_HEIGHT},// min value
  {MESH_INFO_X0 + 1 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 0 * MESH_INFO_HEIGHT, MESH_INFO_X0 + 2 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 1 * MESH_INFO_HEIGHT},// max value
  {MESH_INFO_X0 + 2 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 0 * MESH_INFO_HEIGHT, MESH_INFO_X0 + 3 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 1 * MESH_INFO_HEIGHT},// original value
#endif
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 2 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 3 * MESH_KEY_HEIGHT},        // current value
};

const GUI_RECT meshKeyRect[ME_KEY_NUM] = {
#ifdef MESH_LEFT_KEYBOARD
  {MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 0 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 1 * MESH_KEY_HEIGHT},        // SAVE
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 0 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 1 * MESH_KEY_HEIGHT},        // OK
  {MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 1 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 2 * MESH_KEY_HEIGHT},        // RESET
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 1 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 2 * MESH_KEY_HEIGHT},        // HOME
#else
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 0 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 1 * MESH_KEY_HEIGHT},        // SAVE
  {MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 0 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 1 * MESH_KEY_HEIGHT},        // OK
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 1 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 2 * MESH_KEY_HEIGHT},        // RESET
  {MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 1 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 2 * MESH_KEY_HEIGHT},        // HOME
#endif
  // current value
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 2 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 3 * MESH_KEY_HEIGHT},        // EDIT

  // arrow keys
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 3 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 4 * MESH_KEY_HEIGHT},        // UP
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 4 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 5 * MESH_KEY_HEIGHT},        // PREV
  {MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 4 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 5 * MESH_KEY_HEIGHT},        // NEXT
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 5 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 6 * MESH_KEY_HEIGHT},        // DOWN
};

const GUI_RECT meshAreaRect[ME_AREA_NUM] = {
  {MESH_GRID_X0, MESH_GRID_Y0, MESH_GRID_X0 + MESH_GRID_WIDTH, MESH_GRID_Y0 + MESH_GRID_HEIGHT},             // grid area
  {MESH_INFO_X0, MESH_INFO_Y0, MESH_INFO_X0 + MESH_GRID_WIDTH, MESH_INFO_Y0 + ICON_START_Y},                 // info area
  {MESH_KEY_X0, MESH_KEY_Y0, MESH_KEY_X0 + (LCD_WIDTH - MESH_GRID_WIDTH), MESH_KEY_Y0 + LCD_HEIGHT},         // keyboard area
};

const char *const meshKeyString[ME_KEY_NUM] = {
  "\u08A7",                                                // SAVE
  "\u0894",                                                // OK
  "\u08A5",                                                // RESET
  "\u08A3",                                                // HOME
  "",                                                      // EDIT
  "\u02C4",                                                // UP
  "\u02C2",                                                // PREV
  "\u02C3",                                                // NEXT
  "\u02C5",                                                // DOWN
};

const MESH_DATA_FORMAT meshDataFormat[] = {
  {1, 4, 1, "Mesh Bed Level data:"},                       // MBL
  {0, 2, 0, "Bed Topography Report for CSV:"},             // UBL
  {1, 2, 1, "Bilinear Leveling Grid:"},                    // ABL Bilinear
  {0, 1, 1, "Bed Level Correction Matrix:"},               // ABL Linear or 3-Point
};

void (*meshSaveCallbackPtr)(void) = NULL;
static MESH_DATA *meshData = NULL;

void meshInitData(void)
{
  if (meshData == NULL)
    return;

  meshData->dataSize = 0;
  meshData->colsNum = 0;
  meshData->rowsNum = 0;

  meshData->index = 0;
  meshData->col= 0;
  meshData->row = 0;

  meshData->initValueMinMax = true;
  meshData->valueMin = 0.0f;
  meshData->valueMax = 0.0f;
  meshData->valueDelta = 0.0f;

  uint16_t rgbStart = infoSettings.mesh_min_color;         // RGB color in RGB 565 16 bit format
  uint16_t rgbEnd = infoSettings.mesh_max_color;           // RGB color in RGB 565 16 bit format

  meshData->rStart = (rgbStart >> 11) & 0b0000000000011111;
  meshData->gStart = (rgbStart >> 5) & 0b0000000000111111;
  meshData->bStart = (rgbStart) & 0b0000000000011111;
  meshData->rEnd = (rgbEnd >> 11) & 0b0000000000011111;
  meshData->gEnd = (rgbEnd >> 5) & 0b0000000000111111;
  meshData->bEnd = (rgbEnd) & 0b0000000000011111;
  meshData->rDelta = (float) (meshData->rEnd) - meshData->rStart;
  meshData->gDelta = (float) (meshData->gEnd) - meshData->gStart;
  meshData->bDelta = (float) (meshData->bEnd) - meshData->bStart;

  meshData->status = ME_DATA_IDLE;

  meshData->parsedRows = 0;

  meshSaveCallbackPtr = NULL;
}

static inline void meshAllocData(void)
{
  if (meshData != NULL)                                    // if data already exist (e.g. when the menu is reloaded), continue to use the existing data
    return;

  meshData = (MESH_DATA *) malloc(sizeof(MESH_DATA));

  meshInitData();

  probeHeightEnable();                                     // temporary disable software endstops
}

void meshDeallocData(void)
{
  if (meshData == NULL)
    return;

  free(meshData);

  meshData = NULL;

  probeHeightDisable();                                    // restore original software endstops state
}

bool processKnownDataFormat(char *dataRow)
{
  bool isKnown = false;
  uint8_t i;

  for (i = 0; i < COUNT(meshDataFormat); i++)
  {
    if (strstr(dataRow, meshDataFormat[i].echoMsg))
    {
      isKnown = true;
      break;
    }
  }

  if (isKnown)
  {
    meshData->colsToSkip = meshDataFormat[i].colsToSkip;
    meshData->rowsToSkip = meshDataFormat[i].rowsToSkip;
    meshData->rowsInverted = meshDataFormat[i].rowsInverted;

    meshSaveCallbackPtr = saveEepromSettings;

    switch (infoMachineSettings.leveling)
    {
      case BL_BBL:
        sprintf(meshData->saveTitle, "%s", textSelect(LABEL_ABL_SETTINGS_BBL));
        break;

      case BL_UBL:
        meshSaveCallbackPtr = menuUBLSave;

        sprintf(meshData->saveTitle, "%s", textSelect(LABEL_ABL_SETTINGS_UBL));
        break;

      case BL_MBL:
        sprintf(meshData->saveTitle, "%s", textSelect(LABEL_MBL_SETTINGS));
        break;

      default:
        sprintf(meshData->saveTitle, "%s", textSelect(LABEL_ABL_SETTINGS));
        break;
    }
  }

  return isKnown;
}

void meshSaveCallback(void)
{
  if (meshSaveCallbackPtr != NULL)
    meshSaveCallbackPtr();

  meshDeallocData();                                       // deallocate mesh data. It forces data reloading during Mesh Editor menu reloading
}

bool meshGetStatus(void)
{
  if (meshData->status != ME_DATA_FULL)
    return false;

  return true;
}

static inline uint16_t meshGetSize(void)
{
  if (!meshGetStatus())
    return 0;

  return meshData->dataSize;
}

static inline uint16_t meshGetColsNum(void)
{
  if (!meshGetStatus())
    return 0;

  return meshData->colsNum;
}

static inline uint16_t meshGetRowsNum(void)
{
  if (!meshGetStatus())
    return 0;

  return meshData->rowsNum;
}

static inline uint16_t meshGetIndex(void)
{
  return meshData->index;
}

static inline uint16_t meshGetCol(void)
{
  return meshData->col;
}

static inline uint16_t meshGetRow(void)
{
  return meshData->row;
}

static inline uint16_t meshSetIndex(int32_t index)
{
  if (meshGetStatus())
  {
    index = NOBEYOND(0, index, meshData->dataSize - 1);

    meshData->index = index;
    meshData->col = meshData->index % meshData->colsNum;
    meshData->row = meshData->index / meshData->colsNum;
  }

  return meshData->index;
}

static inline uint16_t meshSetIndexByCol(int32_t col)
{
  if (meshGetStatus())
  {
    col = NOBEYOND(0, col, meshData->colsNum - 1);

    meshData->col = col;
    meshData->index = meshData->row * meshData->colsNum + meshData->col;
  }

  return meshData->index;
}

static inline uint16_t meshSetIndexByRow(int32_t row)
{
  if (meshGetStatus())
  {
    row = NOBEYOND(0, row, meshData->rowsNum - 1);

    meshData->row = row;
    meshData->index = meshData->row * meshData->colsNum + meshData->col;
  }

  return meshData->index;
}

static inline uint16_t meshGetJ(void)
{
  return (meshData->rowsNum - 1) - meshData->row;
}

static inline uint16_t meshGetValueRow(uint16_t index)
{
  if (meshData->rowsInverted)
    return (meshData->rowsNum - 1) - (index / meshData->colsNum);
  else
    return (index / meshData->colsNum);
}

static inline float meshGetValueOrig(uint16_t index)
{
  return meshData->dataOrig[(meshGetValueRow(index) * meshData->colsNum) + (index % meshData->colsNum)];
}

static inline float meshGetValue(uint16_t index)
{
  return meshData->data[(meshGetValueRow(index) * meshData->colsNum) + (index % meshData->colsNum)];
}

static inline bool meshSetValue(float value)
{
  meshData->data[(meshGetValueRow(meshData->index) * meshData->colsNum) + meshData->col] = value;

  storeCmd("M421 I%d J%d Z%.3f\n", meshData->col, meshGetJ(), value);

  return true;
}

static inline float meshGetValueMin(void)
{
  return meshData->valueMin;
}

static inline float meshGetValueMax(void)
{
  return meshData->valueMax;
}

bool meshUpdateValueMinMax(float value)
{
  bool isValueChanged = false;

  if (meshData->initValueMinMax)
  {
    meshData->valueMin = meshData->valueMax = value;
    meshData->initValueMinMax = false;

    isValueChanged = true;
  }
  else if (value < meshData->valueMin)
  {
    meshData->valueMin = value;

    isValueChanged = true;
  }
  else if (value > meshData->valueMax)
  {
    meshData->valueMax = value;

    isValueChanged = true;
  }

  if (isValueChanged)
    meshData->valueDelta = meshData->valueMax - meshData->valueMin;

  return isValueChanged;
}

void meshFullUpdateValueMinMax(void)
{
  if (!meshGetStatus())
    return;

  meshData->initValueMinMax = true;

  for (uint16_t i = 0; i < meshData->dataSize; i++)
  {
    meshUpdateValueMinMax(meshData->data[i]);
  }
}

uint16_t meshGetRGBColor(float value)
{
  if (meshData->valueDelta == 0)
    return (meshData->rStart << 11) | (meshData->gStart << 5) | (meshData->bStart);

  float valueDiff;
  uint16_t r, g, b;

  valueDiff = value - meshData->valueMin;

  r = meshData->rStart + valueDiff * (meshData->rDelta / meshData->valueDelta);
  g = meshData->gStart + valueDiff * (meshData->gDelta / meshData->valueDelta);
  b = meshData->bStart + valueDiff * (meshData->bDelta / meshData->valueDelta);

  return (r << 11) | (g << 5) | (b);
}

void meshDrawGridCell(uint16_t index, uint16_t edgeDistance, bool clearBgColor)
{
  float value = meshGetValue(index);
  uint16_t col = index % meshGetColsNum();
  uint16_t row = index / meshGetColsNum();
  uint16_t cellWidth = MESH_GRID_WIDTH / meshGetColsNum();
  uint16_t cellHeight = MESH_GRID_HEIGHT / meshGetRowsNum();
  uint16_t radius = cellWidth / MESH_POINT_MAX_RATIO;

  if (index == meshGetIndex())
    radius = MIN(cellWidth, cellHeight) / MESH_POINT_MIN_RATIO;
  else if (value == meshGetValueMin() || value == meshGetValueMax())
    radius = MIN(cellWidth, cellHeight) / MESH_POINT_MED_RATIO;

  if (clearBgColor)
    GUI_ClearRect(meshGridRect.x0 + col * cellWidth + 1, meshGridRect.y0 + row * cellHeight + 1,
                  meshGridRect.x0 + (col + 1) * cellWidth - 1, meshGridRect.y0 + (row + 1) * cellHeight - 1);

  GUI_SetColor(infoSettings.list_border_color);

  GUI_DrawLine(meshGridRect.x0 + col * cellWidth + edgeDistance, meshGridRect.y0 + row * cellHeight + cellHeight / 2,
               meshGridRect.x0 + (col + 1) * cellWidth - edgeDistance, meshGridRect.y0 + row * cellHeight + cellHeight / 2);

  GUI_DrawLine(meshGridRect.x0 + col * cellWidth + cellWidth / 2, meshGridRect.y0 + row * cellHeight + edgeDistance,
               meshGridRect.x0 + col * cellWidth + cellWidth / 2, meshGridRect.y0 + (row + 1) * cellHeight - edgeDistance);

  GUI_SetColor(meshGetRGBColor(value));

  GUI_FillCircle(meshGridRect.x0 + col * cellWidth + cellWidth / 2, meshGridRect.y0 + row * cellHeight + cellHeight / 2, radius);

  GUI_SetColor(infoSettings.font_color);
}

void meshDrawGrid(void)
{
  meshFullUpdateValueMinMax();

  GUI_ClearRect(meshGridRect.x0 + 1, meshGridRect.y0 + 1, meshGridRect.x1 - 1, meshGridRect.y1 - 1);

  uint16_t size = meshGetSize();
  for (uint16_t i = 0; i < size; i++)
  {
    meshDrawGridCell(i, MESH_LINE_EDGE_DISTANCE, false);
  }
}

void meshDrawInfoCell(const GUI_RECT *rect, float *val, bool largeFont, uint16_t color, uint16_t bgColor, uint16_t edgeDistance, bool clearBgColor)
{
  uint16_t origBgColor = GUI_GetBkColor();

  if (val != NULL || clearBgColor)
  {
    GUI_SetBkColor(bgColor);
    GUI_ClearRect(rect->x0 + edgeDistance, rect->y0 + edgeDistance, rect->x1 - edgeDistance, rect->y1 - edgeDistance);
  }

  if (val != NULL)
  {
    char tempstr[20];

    sprintf(tempstr, "%.3f", *val);

    GUI_SetColor(color);
    setLargeFont(largeFont);
    GUI_DispStringInPrect(rect, (uint8_t *) tempstr);
    setLargeFont(false);
  }

  GUI_SetColor(infoSettings.font_color);
  GUI_SetBkColor(origBgColor);
}

void meshDrawInfo(float *minVal, float *maxVal, float *origVal, float *curVal)
{
  if (minVal != NULL)
    meshDrawInfoCell(&meshInfoRect[ME_INFO_MIN], minVal, false, meshGetRGBColor(*minVal), infoSettings.bg_color, 1, false);

  if (maxVal != NULL)
    meshDrawInfoCell(&meshInfoRect[ME_INFO_MAX], maxVal, false, meshGetRGBColor(*maxVal), infoSettings.bg_color, 1, false);

  meshDrawInfoCell(&meshInfoRect[ME_INFO_ORIG], origVal, false, infoSettings.font_color, infoSettings.bg_color, 1, false);
  meshDrawInfoCell(&meshInfoRect[ME_INFO_CUR], curVal, true, VALUE_FONT_COLOR, VALUE_BG_COLOR, 2, false);
}

void meshDrawFullInfo(void)
{
  float minValue = meshGetValueMin();
  float maxValue = meshGetValueMax();
  float origValue = meshGetValueOrig(meshGetIndex());
  float curValue = meshGetValue(meshGetIndex());

  meshDrawInfo(&minValue, &maxValue, &origValue, &curValue);
}

void meshKeyPress(uint8_t index, uint8_t isPressed)
{
  if (index >= ME_KEY_NUM)
    return;

  if (isPressed)
  {
    if (index != ME_KEY_EDIT)
      GUI_SetColor(infoSettings.list_border_color);
    else
      GUI_SetColor(VALUE_FONT_COLOR);
  }
  else
  {
    if (index != ME_KEY_EDIT)
      GUI_SetColor(infoSettings.bg_color);
    else
      GUI_SetColor(VALUE_BG_COLOR);
  }

  GUI_DrawRect(meshKeyRect[index].x0 + 2, meshKeyRect[index].y0 + 2, meshKeyRect[index].x1 - 2, meshKeyRect[index].y1 - 2);

  // restore default font color
  GUI_SetColor(infoSettings.font_color);
}

void meshDrawKeyboard(void)
{
  TSC_ReDrawIcon = meshKeyPress;

  setLargeFont(true);

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  for (uint8_t i = 0; i < ME_KEY_NUM; i++)
  {
    if (!(i == ME_KEY_SAVE || i == ME_KEY_OK || i == ME_KEY_RESET || i == ME_KEY_HOME))            // if not a unicode string
      GUI_DispStringInPrect(&meshKeyRect[i], (uint8_t *) meshKeyString[i]);
  }

  if (infoMachineSettings.EEPROM == 1)
    DrawCharIcon(&meshKeyRect[ME_KEY_SAVE], MIDDLE, ICONCHAR_SAVE, false, 0);

  DrawCharIcon(&meshKeyRect[ME_KEY_OK], MIDDLE, ICONCHAR_OK, false, 0);
  DrawCharIcon(&meshKeyRect[ME_KEY_RESET], MIDDLE, ICONCHAR_RESET, false, 0);
  DrawCharIcon(&meshKeyRect[ME_KEY_HOME], MIDDLE, ICONCHAR_MOVE, false, 0);

  GUI_RestoreColorDefault();

  setLargeFont(false);
}

void meshDrawMenu(void)
{
  // clear screen
  GUI_Clear(infoSettings.bg_color);

  GUI_SetColor(infoSettings.list_border_color);

  // draw area borders
  GUI_DrawPrect(&meshAreaRect[ME_AREA_GRID]);              // draw grid area borders
  GUI_DrawPrect(&meshAreaRect[ME_AREA_INFO]);              // draw info area borders
  GUI_DrawPrect(&meshAreaRect[ME_AREA_KEY]);               // draw key borders

  // draw value area borders (outer)
  GUI_DrawRect(meshInfoRect[ME_INFO_CUR].x0, meshInfoRect[ME_INFO_CUR].y0,
               meshInfoRect[ME_INFO_CUR].x1, meshInfoRect[ME_INFO_CUR].y1);

  GUI_SetColor(VALUE_BORDER_COLOR);

  // draw value area borders (inner)
  GUI_DrawRect(meshInfoRect[ME_INFO_CUR].x0 + 1, meshInfoRect[ME_INFO_CUR].y0 + 1,
               meshInfoRect[ME_INFO_CUR].x1 - 1, meshInfoRect[ME_INFO_CUR].y1 - 1);

  GUI_SetColor(infoSettings.font_color);

  // draw grid and keyboard
  meshDrawGrid();
  meshDrawKeyboard();

  // draw values
  if (meshGetStatus())
  {
    float minValue = meshGetValueMin();
    float maxValue = meshGetValueMax();
    float origValue = meshGetValueOrig(meshGetIndex());
    float curValue = meshGetValue(meshGetIndex());

    meshDrawInfo(&minValue, &maxValue, &origValue, &curValue);
  }
}

void meshSave(bool saveOnChange)
{
  if (!meshGetStatus() ||
    (saveOnChange && !memcmp(&meshData->data, &meshData->dataOrig, sizeof(meshData->dataOrig))))   // if no changes, nothing to do
    return;

  if (infoMachineSettings.EEPROM == 1)
  {
    setDialogText((uint8_t *) meshData->saveTitle, LABEL_EEPROM_SAVE_INFO, LABEL_CONFIRM, LABEL_CANCEL);
    showDialog(DIALOG_TYPE_QUESTION, meshSaveCallback, NULL, NULL);
  }
}

bool meshIsWaitingFirstData(void)
{
  if (meshData == NULL ||
    meshData->status != ME_DATA_IDLE)                      // if mesh editor is not running or is already handling data
    return false;

  return true;
}

bool meshIsWaitingData(void)
{
  if (meshData == NULL ||
    meshData->status == ME_DATA_IDLE ||
    meshData->status == ME_DATA_FULL ||
    meshData->status == ME_DATA_FAILED)                    // if mesh editor is not running or is not waiting for data
    return false;

  return true;
}

uint16_t meshParseDataRow(char *dataRow, float *dataGrid, uint16_t maxCount)
{
  if (meshData->parsedRows < meshData->rowsToSkip)
    return 0;

  uint16_t curCount;
  uint16_t validCount;
  char *curPtr;
  char *nextPtr;
  float value;

  curCount = validCount = 0;
  curPtr = nextPtr = dataRow;

  value = strtod(curPtr, &nextPtr);

  if (nextPtr != curPtr)                                   // if a valid float value is parsed
  {
    do
    {
      if (curCount >= meshData->colsToSkip)
      {
        dataGrid[validCount] = value;
        validCount++;
      }

      curCount++;
      curPtr = nextPtr;

      value = strtod(curPtr, &nextPtr);
    }
    while (nextPtr != curPtr && validCount < maxCount);
  }

  return validCount;
}

void meshUpdateData(char *dataRow)
{
  bool failed = false;

  if (meshIsWaitingFirstData())                            // if waiting for first data
  {
    if (processKnownDataFormat(dataRow))                   // if known data format, change state to EMPTY and proceed with data handling
      meshData->status = ME_DATA_EMPTY;
    else
      failed = true;
  }
  else if (!meshIsWaitingData())                           // if not waiting for data, nothing to do
    return;

  if (!failed)
  {
    uint16_t count;

    if (meshData->status == ME_DATA_EMPTY)                 // if data grid is empty, parse the data row and set the data grid columns number
    {
      count = meshParseDataRow(dataRow, &(meshData->dataOrig[0]), MESH_GRID_MAX_POINTS_X);

      if (count > 0)                                       // if number of columns in the parsed data row is at least 1, set the data grid columns number
      {
        meshData->colsNum = count;
        meshData->rowsNum = 1;

        meshData->status = ME_DATA_WAITING;
      }
    }
    else                                                   // if data grid is waiting for the remaining rows, parse the data rows
    {
      count = meshParseDataRow(dataRow, &(meshData->dataOrig[meshData->rowsNum * meshData->colsNum]), meshData->colsNum);

      if (count == meshData->colsNum)                      // if number of columns in the parsed data row matches the data grid columns number, update the data grid rows number
      {
        meshData->rowsNum++;
      }
    }

    meshData->parsedRows++;

    if ((meshData->rowsNum >=1 && count == 0) ||
      meshData->rowsNum == MESH_GRID_MAX_POINTS_Y ||
      meshData->parsedRows >= MESH_MAX_PARSED_ROWS)        // if zero columns are read or maximun rows numnber or maximum parsed rows number is reached
    {
      if (meshData->colsNum != 0 && meshData->rowsNum != 0)// if mesh grid is at least a 1x1 matrix, data grid is marked as full
      {
        meshData->dataSize = meshData->rowsNum * meshData->colsNum;
        meshData->status = ME_DATA_FULL;

        memcpy(&meshData->data, &meshData->dataOrig, sizeof(meshData->dataOrig));
      }
      else                                                 // if mesh grid is smaller than a 1x1 matrix, data grid is marked as failed
        failed = true;
    }
  }

  if (failed)
  {
    meshData->status = ME_DATA_FAILED;

    LABELCHAR(tempMsg, LABEL_PROCESS_ABORTED);

    sprintf(&tempMsg[strlen(tempMsg)], "\n %s", dataRow);

    popupReminder(DIALOG_TYPE_ERROR, LABEL_MESH_EDITOR, (uint8_t *) tempMsg);

    infoMenu.cur--;                                        // exit from mesh editor menu. it avoids to loop in case of persistent error

    meshDeallocData();                                     // deallocate mesh data
  }
}

void menuMeshEditor(void)
{
  MESH_KEY_VALUES key_num = ME_KEY_IDLE;
  bool oldStatus, curStatus;
  uint16_t oldIndex, curIndex;
  float origValue, curValue;
  bool forceHoming;
  bool forceExit;

  meshAllocData();                                         // allocates and initialize mesh data if not already allocated and initialized

  oldStatus = curStatus = meshGetStatus();                 // after allocation, we acces data status etc...
  oldIndex = curIndex = meshGetIndex();
  forceHoming = true;
  forceExit = false;

  mustStoreCmd("M420 V1 T1\n");                            // retrieve the mesh data

  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(meshKeyRect), meshKeyRect, meshKeyPress, &meshDrawMenu);
  meshDrawMenu();

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuMeshEditor)
  {
    curStatus = meshGetStatus();                           // always load current status
    curIndex = meshGetIndex();                             // always load current index

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case ME_KEY_UP:
        curIndex = meshSetIndexByRow(meshGetRow() - 1);
        break;

      case ME_KEY_DOWN:
        curIndex = meshSetIndexByRow(meshGetRow() + 1);
        break;

      case ME_KEY_PREV:
        curIndex = meshSetIndex(curIndex - 1);
        break;

      case ME_KEY_NEXT:
        curIndex = meshSetIndex(curIndex + 1);
        break;

      case ME_KEY_EDIT:
        if (meshGetStatus())
        {
          if (forceHoming)
          {
            forceHoming = false;

            mustStoreCmd("G28\n");                         // only the first time, home the printer
            if (IS_DELTA)
            {
              mustStoreCmd("G91\n");                                  // Set Relative Positioning
              mustStoreCmd("G1 Z-%.2f F1500\n", DELTA_MBL_Z_DROP_MM); // Drop by "DELTA_MBL_Z_DROP_MM" mm
              mustStoreCmd("G90\n");                                  // Set Absolute Positioing
            }
          }

          curValue = menuMeshTuner(meshGetCol(), meshGetJ(), meshGetValue(meshGetIndex()));
          meshSetValue(curValue);

          setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(meshKeyRect), meshKeyRect, meshKeyPress, &meshDrawMenu);
          meshDrawMenu();
        }
        break;

      case ME_KEY_RESET:
        if (meshGetStatus())
        {
          if (meshGetValue(meshGetIndex()) != meshGetValueOrig(meshGetIndex()))
          {
            curValue = meshSetValue(meshGetValueOrig(meshGetIndex()));

            meshDrawGrid();
            meshDrawFullInfo();
          }
        }
        break;

      case ME_KEY_HOME:
        forceHoming = false;

        mustStoreCmd("G28\n");  // force homing (e.g. if steppers are disarmed)
        if (IS_DELTA)
        {
          mustStoreCmd("G91\n");                                   // Set Relative Positioning
          mustStoreCmd("G1 Z-%.2f F1500\n", DELTA_MBL_Z_DROP_MM);  // Drop by "DELTA_MBL_Z_DROP_MM" mm
          mustStoreCmd("G90\n");                                   // Set Absolute Positioing
        }
        break;

      case ME_KEY_SAVE:
        meshSave(false);
        break;

      case ME_KEY_OK:
        forceExit = true;

        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            curIndex = meshSetIndex(curIndex + encoderPosition);

            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (meshGetStatus() && (oldStatus != curStatus || oldIndex != curIndex))
    {
      if (oldStatus != curStatus)
      {
        meshDrawGrid();
        meshDrawFullInfo();
      }
      else
      {
        meshDrawGridCell(oldIndex, MESH_LINE_EDGE_DISTANCE, true);             // draw point with old index
        meshDrawGridCell(curIndex, MESH_LINE_EDGE_DISTANCE, true);             // draw point with new index

        origValue = meshGetValueOrig(meshGetIndex());
        curValue = meshGetValue(meshGetIndex());

        meshDrawInfo(NULL, NULL, &origValue, &curValue);
      }

      oldStatus = curStatus;
      oldIndex = curIndex;
    }

    loopProcess();
  }

  if (forceExit)
  {
    meshSave(true);                                        // check for changes and ask to save on eeprom

    meshDeallocData();                                     // deallocate mesh data
  }
}
