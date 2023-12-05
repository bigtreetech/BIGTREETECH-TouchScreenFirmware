#include "MeshEditor.h"
#include "includes.h"

// value ranges
#define MESH_GRID_SIZE             (MESH_GRID_MAX_POINTS_X * MESH_GRID_MAX_POINTS_Y)
#define MESH_MAX_PARSED_ROWS       30  // maximum number of data rows to parse for retrieving the mesh
                                       // grid from the "M420 T1 V1" command output provided by the Marlin FW

#define MESH_MAX_RETRIEVE_ATTEMPTS 20  // maximum number of attempts to retrieve the data format from Marlin FW
#define MESH_LINE_EDGE_DISTANCE    4

// data structures
typedef struct
{
  const uint8_t colsToSkip;
  const uint8_t rowsToSkip;
  const bool rowsInverted;
  const char * const echoMsg;
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
  float oriData[MESH_GRID_SIZE];  // data grid of original Z height
  float curData[MESH_GRID_SIZE];  // data grid of current Z height

  uint16_t dataSize;              // number of data present in the data grid
  uint16_t colsNum;               // number of points per X axis (number of columns in the data grid)
  uint16_t rowsNum;               // number of points per Y axis (number of rows in the data grid)

  uint16_t index;                 // current index in the data grid
  uint16_t col;                   // current column in the data grid
  uint16_t row;                   // current row in the data grid

  float valueMin;                 // minimum data value in the data grid
  float valueMax;                 // maximum data value in the data grid
  float valueDelta;               // valueMax - valueMin

  uint16_t rStart;                // RGB red component for drawing the minimum data value in the data grid
  uint16_t gStart;                // RGB green component for drawing the minimum data value in the data grid
  uint16_t bStart;                // RGB blue component for drawing the minimum data value in the data grid
  uint16_t rEnd;                  // RGB green component for drawing the maximum data value in the data grid
  uint16_t gEnd;                  // RGB red component for drawing the maximum data value in the data grid
  uint16_t bEnd;                  // RGB blue component for drawing the maximum data value in the data grid
  int16_t rDelta;                 // rEnd - rStart
  int16_t gDelta;                 // gEnd - gStart
  int16_t bDelta;                 // bEnd - bStart

  MESH_DATA_STATUS status;        // current status of oriData/curData
  uint8_t parsedRows;

  uint8_t colsToSkip;
  uint8_t rowsToSkip;
  bool rowsInverted;

  char saveTitle[120];
} MESH_DATA;

enum
{
  ME_INFO_MIN = 0,
  ME_INFO_MAX,
  ME_INFO_ORIG,
  ME_INFO_CUR,
  ME_INFO_NUM,  // number of infos
  ME_INFO_IDLE = IDLE_TOUCH,
};

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
  ME_KEY_NUM,  // number of keys
  ME_KEY_INCREASE = KEY_INCREASE,
  ME_KEY_DECREASE = KEY_DECREASE,
  ME_KEY_IDLE = IDLE_TOUCH,
} MESH_KEY_VALUES;

enum
{
  ME_AREA_GRID = 0,
  ME_AREA_INFO,
  ME_AREA_KEY,
  #ifdef PORTRAIT_MODE
    ME_AREA_ARROW,
  #endif
  ME_AREA_NUM,  // number of areas
  ME_AREA_IDLE = IDLE_TOUCH,
};

// layout sizes
#ifdef PORTRAIT_MODE
  #define MESH_GRID_HEIGHT   (LCD_WIDTH - (LCD_WIDTH / 3))
  #define MESH_GRID_WIDTH    MESH_GRID_HEIGHT
#else
  #define MESH_GRID_HEIGHT   (LCD_HEIGHT - ICON_START_Y)
  #define MESH_GRID_WIDTH    MESH_GRID_HEIGHT
#endif
#define MESH_POINT_MIN_RATIO 3
#define MESH_POINT_MED_RATIO 5
#define MESH_POINT_MAX_RATIO 8

#define MESH_INFO_ROW_NUM 1
#define MESH_INFO_COL_NUM 3
#define MESH_INFO_HEIGHT  (ICON_START_Y / MESH_INFO_ROW_NUM)
#define MESH_INFO_WIDTH   (MESH_GRID_WIDTH / MESH_INFO_COL_NUM)

#ifdef PORTRAIT_MODE
  #define MESH_KEY_ROW_NUM 5
  #define MESH_KEY_COL_NUM 1
  #define MESH_KEY_HEIGHT  (LCD_HEIGHT / MESH_KEY_ROW_NUM)
  #define MESH_KEY_WIDTH   ((LCD_WIDTH - MESH_GRID_WIDTH) / MESH_KEY_COL_NUM)
#else
  #define MESH_KEY_ROW_NUM 6
  #define MESH_KEY_COL_NUM 2
  #define MESH_KEY_HEIGHT  (LCD_HEIGHT / MESH_KEY_ROW_NUM)
  #define MESH_KEY_WIDTH   ((LCD_WIDTH - MESH_GRID_WIDTH) / MESH_KEY_COL_NUM)
#endif

#ifdef KEYBOARD_ON_LEFT
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

#ifdef PORTRAIT_MODE
  #define MESH_ARROW_HEIGHT ((LCD_HEIGHT - (ICON_START_Y + MESH_GRID_HEIGHT)) / 3)
  #define MESH_ARROW_WIDTH  (MESH_GRID_WIDTH / 2)
  #define MESH_ARROW_Y0     (ICON_START_Y + MESH_GRID_HEIGHT)
  #ifdef KEYBOARD_ON_LEFT
    #define MESH_ARROW_X0   MESH_ARROW_WIDTH
  #else
    #define MESH_ARROW_X0   0
  #endif
#endif

const GUI_RECT meshGridRect = {MESH_GRID_X0, MESH_GRID_Y0, MESH_GRID_X0 + MESH_GRID_WIDTH, MESH_GRID_Y0 + MESH_GRID_HEIGHT};

const GUI_RECT meshInfoRect[ME_INFO_NUM] = {
#ifdef KEYBOARD_ON_LEFT
  {MESH_INFO_X0 + 1 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 0 * MESH_INFO_HEIGHT, MESH_INFO_X0 + 2 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 1 * MESH_INFO_HEIGHT},  // min value
  {MESH_INFO_X0 + 2 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 0 * MESH_INFO_HEIGHT, MESH_INFO_X0 + 3 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 1 * MESH_INFO_HEIGHT},  // max value
  {MESH_INFO_X0 + 0 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 0 * MESH_INFO_HEIGHT, MESH_INFO_X0 + 1 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 1 * MESH_INFO_HEIGHT},  // original value
#else
  {MESH_INFO_X0 + 0 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 0 * MESH_INFO_HEIGHT, MESH_INFO_X0 + 1 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 1 * MESH_INFO_HEIGHT},  // min value
  {MESH_INFO_X0 + 1 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 0 * MESH_INFO_HEIGHT, MESH_INFO_X0 + 2 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 1 * MESH_INFO_HEIGHT},  // max value
  {MESH_INFO_X0 + 2 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 0 * MESH_INFO_HEIGHT, MESH_INFO_X0 + 3 * MESH_INFO_WIDTH, MESH_INFO_Y0 + 1 * MESH_INFO_HEIGHT},  // original value
#endif

  // current value
#ifdef PORTRAIT_MODE
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 4 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 5 * MESH_KEY_HEIGHT},          // current value
#else
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 2 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 3 * MESH_KEY_HEIGHT},          // current value
#endif
};

const GUI_RECT meshKeyRect[ME_KEY_NUM] = {
#ifdef PORTRAIT_MODE
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 1 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 2 * MESH_KEY_HEIGHT},          // SAVE
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 0 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 1 * MESH_KEY_HEIGHT},          // OK
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 2 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 3 * MESH_KEY_HEIGHT},          // RESET
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 3 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 4 * MESH_KEY_HEIGHT},          // HOME

  // current value
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 4 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 5 * MESH_KEY_HEIGHT},          // current value

  // arrow keys
  {MESH_ARROW_X0 + 0 * MESH_ARROW_WIDTH, MESH_ARROW_Y0 + 0 * MESH_ARROW_HEIGHT, MESH_ARROW_X0 + 2 * MESH_ARROW_WIDTH, MESH_ARROW_Y0 + 1 * MESH_ARROW_HEIGHT},  // UP
  {MESH_ARROW_X0 + 0 * MESH_ARROW_WIDTH, MESH_ARROW_Y0 + 1 * MESH_ARROW_HEIGHT, MESH_ARROW_X0 + 1 * MESH_ARROW_WIDTH, MESH_ARROW_Y0 + 2 * MESH_ARROW_HEIGHT},  // PREV
  {MESH_ARROW_X0 + 1 * MESH_ARROW_WIDTH, MESH_ARROW_Y0 + 1 * MESH_ARROW_HEIGHT, MESH_ARROW_X0 + 2 * MESH_ARROW_WIDTH, MESH_ARROW_Y0 + 2 * MESH_ARROW_HEIGHT},  // NEXT
  {MESH_ARROW_X0 + 0 * MESH_ARROW_WIDTH, MESH_ARROW_Y0 + 2 * MESH_ARROW_HEIGHT, MESH_ARROW_X0 + 2 * MESH_ARROW_WIDTH, MESH_ARROW_Y0 + 3 * MESH_ARROW_HEIGHT},  // DOWN
#else
  #ifdef KEYBOARD_ON_LEFT
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
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 2 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 3 * MESH_KEY_HEIGHT},          // EDIT

  // arrow keys
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 3 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 4 * MESH_KEY_HEIGHT},          // UP
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 4 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 5 * MESH_KEY_HEIGHT},          // PREV
  {MESH_KEY_X0 + 1 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 4 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 5 * MESH_KEY_HEIGHT},          // NEXT
  {MESH_KEY_X0 + 0 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 5 * MESH_KEY_HEIGHT, MESH_KEY_X0 + 2 * MESH_KEY_WIDTH, MESH_KEY_Y0 + 6 * MESH_KEY_HEIGHT},          // DOWN
#endif
};

const GUI_RECT meshAreaRect[ME_AREA_NUM] = {
  {MESH_GRID_X0, MESH_GRID_Y0, MESH_GRID_X0 + MESH_GRID_WIDTH, MESH_GRID_Y0 + MESH_GRID_HEIGHT},  // grid area
  {MESH_INFO_X0, MESH_INFO_Y0, MESH_INFO_X0 + MESH_GRID_WIDTH, MESH_GRID_Y0},                     // info area
  {MESH_KEY_X0, MESH_KEY_Y0, MESH_KEY_X0 + (LCD_WIDTH - MESH_GRID_WIDTH), LCD_HEIGHT},            // keyboard area
#ifdef PORTRAIT_MODE
  {MESH_ARROW_X0, MESH_ARROW_Y0, MESH_ARROW_X0 + MESH_GRID_WIDTH, LCD_HEIGHT},                    // arrow area
#endif
};

const char * const meshKeyString[ME_KEY_NUM] = {
  "\u08A7",  // SAVE
  "\u0894",  // OK
  "\u08A5",  // RESET
  "\u08A3",  // HOME
  "",        // EDIT
  "\u02C4",  // UP
  "\u02C2",  // PREV
  "\u02C3",  // NEXT
  "\u02C5",  // DOWN
};

const MESH_DATA_FORMAT meshDataFormat[] = {
  // columns to skip, rows to skip, rows inverted, bed leveling data type
  {                1,            4,          true, "Mesh Bed Level data:"},            // MBL
  {                0,            2,         false, "Bed Topography Report for CSV:"},  // UBL
  {                1,            2,          true, "Bilinear Leveling Grid:"},         // ABL Bilinear
  {                0,            1,          true, "Bed Level Correction Matrix:"},    // ABL Linear or 3-Point
};

const char * meshErrorMsg[] = {"Invalid mesh"};  // list of possible error responses to "M420 V1 T1" command

static MESH_DATA * meshData = NULL;

static inline void meshInitData(void)
{
  if (meshData == NULL)
    return;

  memset(meshData, 0, sizeof(MESH_DATA));

  uint16_t rgbStart = infoSettings.mesh_min_color;  // RGB color in RGB 565 16 bit format
  uint16_t rgbEnd = infoSettings.mesh_max_color;    // RGB color in RGB 565 16 bit format

  meshData->rStart = (rgbStart >> 11) & 0b0000000000011111;
  meshData->gStart = (rgbStart >> 5)  & 0b0000000000111111;
  meshData->bStart = (rgbStart)       & 0b0000000000011111;
  meshData->rEnd =   (rgbEnd >> 11)   & 0b0000000000011111;
  meshData->gEnd =   (rgbEnd >> 5)    & 0b0000000000111111;
  meshData->bEnd =   (rgbEnd)         & 0b0000000000011111;
  meshData->rDelta = meshData->rEnd - meshData->rStart;
  meshData->gDelta = meshData->gEnd - meshData->gStart;
  meshData->bDelta = meshData->bEnd - meshData->bStart;

  // meshData->status = ME_DATA_IDLE;  // init already done by memset() (ME_DATA_IDLE is 0 as the value used by memset())
}

static inline void meshAllocData(void)
{
  // if data doesn't already exist (e.g. when the menu is loaded for the first time), initialize data.
  // Otherwise, if data already exist (e.g. when the menu is reloaded), continue to use the existing data
  if (meshData == NULL)
  {
    meshData = (MESH_DATA *) malloc(sizeof(MESH_DATA));
    meshInitData();

    probeHeightEnable();  // temporary disable software endstops and save ABL state
  }
}

void meshDeallocData(void)
{
  if (meshData != NULL)
  {
    free(meshData);
    meshData = NULL;

    probeHeightDisable();  // restore original software endstops state and ABL state
  }
}

static inline bool processKnownErrorMessage(const char * dataRow)
{
  uint8_t dataCount = COUNT(meshErrorMsg);

  for (uint8_t i = 0; i < dataCount; i++ )
  {
    if (strstr(dataRow, meshErrorMsg[i]) != NULL)
      return true;
  }

  return false;
}

static inline bool processKnownDataFormat(const char * dataRow)
{
  uint8_t dataCount = COUNT(meshDataFormat);

  for (uint8_t i = 0; i < dataCount; i++ )
  {
    if (strstr(dataRow, meshDataFormat[i].echoMsg))
    {
      char * title;

      meshData->colsToSkip = meshDataFormat[i].colsToSkip;
      meshData->rowsToSkip = meshDataFormat[i].rowsToSkip;
      meshData->rowsInverted = meshDataFormat[i].rowsInverted;

      switch (infoMachineSettings.leveling)
      {
        case BL_BBL:
          title = (char *)textSelect(LABEL_ABL_SETTINGS_BBL);
          break;

        case BL_UBL:
          title = (char *)textSelect(LABEL_ABL_SETTINGS_UBL);
          break;

        case BL_MBL:
          title = (char *)textSelect(LABEL_MBL_SETTINGS);
          break;

        default:
          title = (char *)textSelect(LABEL_ABL_SETTINGS);
          break;
      }

      strcpy(meshData->saveTitle, title);
      return true;
    }
  }

  return false;
}

void meshSaveCallback(void)
{
  if (infoMachineSettings.leveling == BL_UBL)
    menuUBLSave();
  else if (infoMachineSettings.leveling != BL_DISABLED)
    saveEepromSettings();

  if (meshData != NULL)  // if data have not been released (e.g. data are released when mesh editor menu is forced to exit)
    memcpy(meshData->oriData, meshData->curData, sizeof(meshData->curData));  // align data only after save confirmation
}

static inline void meshUpdateIndex(const MESH_KEY_VALUES key_num)
{
  switch (key_num)
  {
    case ME_KEY_UP:
      meshData->row = (meshData->rowsNum + meshData->row - 1) % meshData->rowsNum;
      break;

    case ME_KEY_DOWN:
      meshData->row = (meshData->row + 1) % meshData->rowsNum;
      break;

    case ME_KEY_PREV:
    case ME_KEY_DECREASE:
    case ME_KEY_NEXT:
    case ME_KEY_INCREASE:
    {
      uint16_t index;

      if (key_num == ME_KEY_PREV || key_num == ME_KEY_DECREASE)
        index = meshData->index > 0 ? meshData->index - 1 : meshData->dataSize - 1;
      else
        index = meshData->index < meshData->dataSize - 1 ? meshData->index + 1 : 0;

      meshData->col = index % meshData->colsNum;
      meshData->row = index / meshData->colsNum;
      break;
    }

    default:
      break;
  }

  meshData->index = meshData->row * meshData->colsNum + meshData->col;
}

uint16_t meshGetJ(void)
{
  // J index (independent by data format) to be used by G42 (mesh tuner menu) and M421 (meshSetValue() function).
  // Bed's top left point -> J = max row index
  // Bed's bottom left point -> J = 0
  return (meshData->rowsNum - 1) - meshData->row;
}

bool meshSetValue(const float value)
{
  if (meshData->curData[meshData->index] != value)  // if mesh value is changed
  {
    meshData->curData[meshData->index] = value;  // set new mesh value

    mustStoreCmd("M421 I%d J%d Z%.3f\n", meshData->col, meshGetJ(), value);  // send new mesh value

    return true;
  }

  return false;
}

static inline void meshUpdateValueMinMax(void)
{
  float value;

  meshData->valueMin = meshData->valueMax = meshData->curData[0];  // init initial min/max values

  for (uint16_t i = 0; i < meshData->dataSize; i++)
  {
    value = meshData->curData[i];

    if (value < meshData->valueMin)
      meshData->valueMin = value;
    else if (value > meshData->valueMax)
      meshData->valueMax = value;
  }

  meshData->valueDelta = meshData->valueMax - meshData->valueMin;
}

uint16_t mapRGBdata(const uint16_t * rgbStart, const int16_t * rgbDelta, const float * valueDiff, const float * valueDelta)
{
  return *rgbStart + (*valueDiff * *rgbDelta) / *valueDelta;
}

uint16_t meshGetRGBColor(const float value)
{
  if (meshData->valueDelta == 0)
    return (meshData->rStart << 11) | (meshData->gStart << 5) | (meshData->bStart);

  float valueDiff = value - meshData->valueMin;
  uint16_t r = mapRGBdata(&meshData->rStart, &meshData->rDelta, &valueDiff, &meshData->valueDelta);
  uint16_t g = mapRGBdata(&meshData->gStart, &meshData->gDelta, &valueDiff, &meshData->valueDelta);
  uint16_t b = mapRGBdata(&meshData->bStart, &meshData->bDelta, &valueDiff, &meshData->valueDelta);

  return (r << 11) | (g << 5) | (b);
}

void meshDrawGridCell(const uint16_t index, const bool clearBgColor)
{
  float value = meshData->curData[index];
  uint16_t col = index % meshData->colsNum;
  uint16_t row = index / meshData->colsNum;
  uint16_t cellWidth = MESH_GRID_WIDTH / meshData->colsNum;
  uint16_t cellHeight = MESH_GRID_HEIGHT / meshData->rowsNum;
  uint16_t radius = MIN(cellWidth, cellHeight);
  uint16_t rect_x0 = meshGridRect.x0 + col * cellWidth;
  uint16_t rect_y0 = meshGridRect.y0 + row * cellHeight;

  if (index == meshData->index)
    radius /= MESH_POINT_MIN_RATIO;
  else if (value == meshData->valueMin || value == meshData->valueMax)
    radius /= MESH_POINT_MED_RATIO;
  else
    radius /= MESH_POINT_MAX_RATIO;

  if (clearBgColor)
    GUI_ClearRect(rect_x0 + 1, rect_y0 + 1,
                  rect_x0 + cellWidth - 1, rect_y0 + cellHeight - 1);

  GUI_SetColor(MESH_BORDER_COLOR);

  GUI_DrawLine(rect_x0 + MESH_LINE_EDGE_DISTANCE, rect_y0 + cellHeight / 2,
               rect_x0 + cellWidth - MESH_LINE_EDGE_DISTANCE, rect_y0 + cellHeight / 2);

  GUI_DrawLine(rect_x0 + cellWidth / 2, rect_y0 + MESH_LINE_EDGE_DISTANCE,
               rect_x0 + cellWidth / 2, rect_y0 + cellHeight - MESH_LINE_EDGE_DISTANCE);

  GUI_SetColor(meshGetRGBColor(value));

  GUI_FillCircle(rect_x0 + cellWidth / 2, rect_y0 + cellHeight / 2, radius);

  GUI_SetColor(MESH_FONT_COLOR);
}

void meshDrawGrid(void)
{
  meshUpdateValueMinMax();

  drawBackground(&meshGridRect, MESH_BG_COLOR, 1);

  for (uint16_t i = 0; i < meshData->dataSize; i++)
  {
    meshDrawGridCell(i, false);
  }
}

void meshDrawInfo(const bool drawMinMax)
{
  if (drawMinMax)
  {
    drawStandardValue(&meshInfoRect[ME_INFO_MIN], VALUE_FLOAT, &meshData->valueMin, FONT_SIZE_NORMAL, meshGetRGBColor(meshData->valueMin), MESH_BG_COLOR, 1, true);
    drawStandardValue(&meshInfoRect[ME_INFO_MAX], VALUE_FLOAT, &meshData->valueMax, FONT_SIZE_NORMAL, meshGetRGBColor(meshData->valueMax), MESH_BG_COLOR, 1, true);
  }

  drawStandardValue(&meshInfoRect[ME_INFO_ORIG], VALUE_FLOAT, &meshData->oriData[meshData->index], FONT_SIZE_NORMAL, MESH_FONT_COLOR, MESH_BG_COLOR, 1, true);
  drawStandardValue(&meshInfoRect[ME_INFO_CUR], VALUE_FLOAT, &meshData->curData[meshData->index], FONT_SIZE_LARGE, MESH_FONT_COLOR, MESH_BORDER_COLOR, 4, true);
}

static inline void meshDrawButton(const uint8_t index, const uint8_t isPressed)
{
  if (index >= ME_KEY_NUM)
    return;

  uint16_t color;

  if (isPressed)
  {
    if (index != ME_KEY_EDIT)
      color = MESH_BORDER_COLOR;
    else
      color = MESH_FONT_COLOR;
  }
  else
  {
    if (index != ME_KEY_EDIT)
      color = MESH_BG_COLOR;
    else
      color = MESH_BORDER_COLOR;
  }

  drawBorder(&meshKeyRect[index], color, 1);

  // restore default font color
  GUI_SetColor(MESH_FONT_COLOR);
}

static inline void meshDrawKeyboard(void)
{
  // draw buttons
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  for (uint8_t i = 0; i < ME_KEY_NUM; i++)
  {
    if (i > ME_KEY_EDIT)  // if not a unicode string
      drawStandardValue(&meshKeyRect[i], VALUE_STRING, meshKeyString[i], FONT_SIZE_LARGE, MESH_FONT_COLOR, MESH_BG_COLOR, 3, true);
  }

  // draw control icons
  if (infoMachineSettings.EEPROM == 1)
    drawCharIcon(&meshKeyRect[ME_KEY_SAVE], CENTER, CHARICON_SAVE, false, 0);

  drawCharIcon(&meshKeyRect[ME_KEY_OK], CENTER, CHARICON_OK, false, 0);
  drawCharIcon(&meshKeyRect[ME_KEY_RESET], CENTER, CHARICON_RESET, false, 0);
  drawCharIcon(&meshKeyRect[ME_KEY_HOME], CENTER, CHARICON_MOVE, false, 0);

  // restore default
  GUI_RestoreColorDefault();
}

void meshDrawMenu(void)
{
  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(meshKeyRect), meshKeyRect, meshDrawButton, &meshDrawMenu);

  // clear screen
  GUI_Clear(MESH_BG_COLOR);

  GUI_SetColor(MESH_BORDER_COLOR);

  // draw area borders
  GUI_DrawPrect(&meshAreaRect[ME_AREA_GRID]);     // draw grid area borders
  GUI_DrawPrect(&meshAreaRect[ME_AREA_INFO]);     // draw info area borders
  GUI_DrawPrect(&meshAreaRect[ME_AREA_KEY]);      // draw key borders
  #ifdef PORTRAIT_MODE
    GUI_DrawPrect(&meshAreaRect[ME_AREA_ARROW]);  // draw arrow area borders
  #endif

  // draw value area borders (outer)
  drawBorder(&meshInfoRect[ME_INFO_CUR], MESH_BORDER_COLOR, 2);

  // draw value area borders (inner)
  drawBorder(&meshInfoRect[ME_INFO_CUR], MESH_BORDER_COLOR_2, 3);

  // restore default font color
  GUI_SetColor(MESH_FONT_COLOR);

  // draw grid and keyboard
  meshDrawGrid();
  meshDrawKeyboard();

  // draw values
  meshDrawInfo(true);
}

void meshSave(void)
{
  if (infoMachineSettings.EEPROM == 1)
    popupDialog(DIALOG_TYPE_QUESTION, (uint8_t *) meshData->saveTitle, LABEL_EEPROM_SAVE_INFO, LABEL_CONFIRM, LABEL_CANCEL, meshSaveCallback, NULL, NULL);
}

static inline bool meshIsWaitingFirstData(void)
{ // just avoid to merge on the same "if" statement a check on NULL value and an access
  // to attributes of the data structure meshData due to different compiler optimization
  // settings (that could evaluate all the conditions in the "if" statement, causing a crash)
  if (meshData == NULL)  // if mesh editor is not running
    return false;

  if (meshData->status != ME_DATA_IDLE)  // if mesh editor is already handling data
    return false;

  return true;
}

bool meshIsWaitingData(void)
{ // just avoid to merge on the same "if" statement a check on NULL value and an access
  // to attributes of the data structure meshData due to different compiler optimization
  // settings (that could evaluate all the conditions in the "if" statement, causing a crash)
  if (meshData == NULL)  // if mesh editor is not running
    return false;

  if (meshData->status == ME_DATA_FULL || meshData->status == ME_DATA_FAILED)  // if not waiting for data
    return false;

  return true;
}

uint16_t meshParseDataRow(char * dataRow, float * dataGrid, const uint16_t maxCount)
{
  if (meshData->parsedRows < meshData->rowsToSkip)
    return 0;

  uint16_t count;
  char * curPtr;
  char * nextPtr;
  float value;

  count = 0;
  nextPtr = dataRow;

  do  // skip leading columns
  {
    curPtr = nextPtr;
    value = strtod(curPtr, &nextPtr);
  } while (count++ < meshData->colsToSkip);

  count = 0;

  while (nextPtr != curPtr && count < maxCount)  // parse valid floats
  {
    dataGrid[count] = value;
    count++;
    curPtr = nextPtr;
    value = strtod(curPtr, &nextPtr);
  }

  return count;
}

static inline void processGridData(void)
{
  if (meshData->rowsInverted)  // store grid data with the rows in inverse order
  {
    for (uint8_t i = 0; i < meshData->rowsNum; i++)  // copy oriData to curData with the rows in inverse order
    {
      memcpy(&(meshData->curData[i * meshData->colsNum]),
             &(meshData->oriData[meshData->dataSize - (i + 1) * meshData->colsNum]),
             sizeof(meshData->oriData[0]) * meshData->colsNum);
    }

    memcpy(meshData->oriData, meshData->curData, sizeof(meshData->curData));  // copy the inversed rows data back to oriData
  }
  else  // copy oriData to curData
  {
    memcpy(meshData->curData, meshData->oriData, sizeof(meshData->oriData));
  }
}

void meshUpdateData(char * dataRow)
{
  bool failed = false;

  if (meshIsWaitingFirstData())  // if waiting for mesh type/format data
  {
    meshData->parsedRows++;

    if (processKnownErrorMessage(dataRow) || meshData->parsedRows > MESH_MAX_RETRIEVE_ATTEMPTS)  // mesh error or max try exceeded
    {
      failed = true;
    }
    else
    {
      if (processKnownDataFormat(dataRow))  // if known data format, change state to EMPTY to enable the mesh parsing
      {
        meshData->status = ME_DATA_EMPTY;

        meshData->parsedRows = 1;
      }

      return;
    }
  }
  else if (!meshIsWaitingData())  // if not waiting for data, nothing to do
  {
    return;
  }

  if (!failed)
  {
    uint16_t count;

    if (meshData->status == ME_DATA_EMPTY)  // if data grid is empty, parse the data row and set the data grid columns number
    {
      count = meshParseDataRow(dataRow, &(meshData->oriData[0]), MESH_GRID_MAX_POINTS_X);

      if (count > 0)  // if number of columns in the parsed data row is at least 1, set the data grid columns number
      {
        meshData->status = ME_DATA_WAITING;

        meshData->colsNum = count;
        meshData->rowsNum = 1;
      }
    }
    else  // if data grid is waiting for the remaining rows, parse the data rows
    {
      count = meshParseDataRow(dataRow, &(meshData->oriData[meshData->rowsNum * meshData->colsNum]), meshData->colsNum);

      // if number of columns in the parsed data row matches the data grid columns number, update the data grid rows number
      if (count == meshData->colsNum)
      {
        meshData->rowsNum++;
      }
    }

    meshData->parsedRows++;

    // if zero columns are read or maximun rows numnber or maximum parsed rows number is reached
    if ((meshData->rowsNum >= 1 && count == 0) ||
      meshData->rowsNum == MESH_GRID_MAX_POINTS_Y ||
      meshData->parsedRows >= MESH_MAX_PARSED_ROWS)
    {
      if (meshData->colsNum == 0 || meshData->rowsNum == 0)  // if mesh grid is smaller than a 1x1 matrix, data grid is marked as failed
      {
        failed = true;
      }
      else  // if mesh grid is at least a 1x1 matrix, data grid is marked as full
      {
        meshData->status = ME_DATA_FULL;
        meshData->dataSize = meshData->rowsNum * meshData->colsNum;

        processGridData();
      }
    }
  }

  if (failed)
  {
    meshData->status = ME_DATA_FAILED;

    char tempMsg[MAX_STRING_LENGTH];

    snprintf(tempMsg, MAX_STRING_LENGTH, "%s\n-> %s", textSelect(LABEL_PROCESS_ABORTED), dataRow);

    popupReminder(DIALOG_TYPE_ERROR, LABEL_MESH_EDITOR, (uint8_t *) tempMsg);

    // trigger exit from mesh editor menu. It avoids to loop in case of persistent error
    CLOSE_MENU();
  }
}

void menuMeshEditor(void)
{
  MESH_KEY_VALUES key_num = ME_KEY_IDLE;
  uint16_t curIndex;

  meshAllocData();  // allocates and initialize mesh data if not already allocated and initialized
  curIndex = meshData->index;

  mustStoreCmd("M420 V1 T1\n");  // retrieve the mesh data

  // wait for mesh data to fully load before entering the menu or exit if any error occurs
  while (meshData->status != ME_DATA_FULL)
  {
    if (meshData->status == ME_DATA_FAILED)
    {
      meshDeallocData();
      return;
    }

    loopProcess();
  }

  meshDrawMenu();

  while (MENU_IS(menuMeshEditor))
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case ME_KEY_UP:
      case ME_KEY_DOWN:
      case ME_KEY_PREV:
      case ME_KEY_DECREASE:
      case ME_KEY_NEXT:
      case ME_KEY_INCREASE:
        meshUpdateIndex(key_num);

        meshDrawGridCell(curIndex, true);  // draw point with old index
        curIndex = meshData->index;        // update current index with new index
        meshDrawGridCell(curIndex, true);  // draw point with new index
        meshDrawInfo(false);
        break;

      case ME_KEY_EDIT:
        if (coordinateIsKnown() == false)
          probeHeightHome();  // home, disable ABL and raise nozzle

        // call mesh tuner menu and set current mesh value, if changed
        meshSetValue(menuMeshTuner(meshData->col, meshGetJ(), meshData->curData[curIndex]));

        meshDrawMenu();
        break;

      case ME_KEY_RESET:
        if (meshSetValue(meshData->oriData[curIndex]))  // set current mesh value to original value, if changed
        {
          meshDrawGrid();
          meshDrawInfo(true);
        }
        break;

      case ME_KEY_HOME:
        probeHeightHome();  // force homing (e.g. if steppers are disarmed)
        break;

      case ME_KEY_SAVE:
        // we unconditionally allow to save the meshes on EEPROM (e.g. just in case we previously edited some
        // meshes but we closed the mesh editor menu refusing to save)
        meshSave();
        break;

      case ME_KEY_OK:
        if (memcmp(meshData->oriData, meshData->curData, sizeof(meshData->curData)))  // check for changes
          meshSave();

        meshDeallocData();  // finally, deallocate mesh data (meshData no more accessible)

        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }

  // restore default
  GUI_RestoreColorDefault();
}
