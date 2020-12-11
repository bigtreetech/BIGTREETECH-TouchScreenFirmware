#ifndef _MESH_EDITOR_H_
#define _MESH_EDITOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "Configuration.h"
#include "menu.h"

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

/* called by parseAck() */
bool meshIsWaitingFirstData(void);
bool meshIsWaitingData(void);
void meshUpdateData(char *dataRow);

void menuMeshEditor(void);

#ifdef __cplusplus
}
#endif

#endif
