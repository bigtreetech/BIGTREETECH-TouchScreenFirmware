#include "SerialConnection.h"
#include "includes.h"

#define SERIAL_PORT_RX_QUEUE_SIZE   NOBEYOND(256, RAM_SIZE * 64, 4096)  // ACK messages reading queue from mainboard
#define SERIAL_PORT_2_RX_QUEUE_SIZE 512
#define SERIAL_PORT_3_RX_QUEUE_SIZE 512
#define SERIAL_PORT_4_RX_QUEUE_SIZE 512

#define SERIAL_PORT_TX_QUEUE_SIZE   256  // gcodes writing queue to mainboard
#define SERIAL_PORT_2_TX_QUEUE_SIZE 256
#define SERIAL_PORT_3_TX_QUEUE_SIZE 256
#define SERIAL_PORT_4_TX_QUEUE_SIZE 256

const SERIAL_PORT_INFO serialPort[SERIAL_PORT_COUNT] = {
  {SERIAL_PORT    , SERIAL_PORT_RX_QUEUE_SIZE  , SERIAL_PORT_TX_QUEUE_SIZE  , "" , "1 - Printer"},
  #ifdef SERIAL_PORT_2
    {SERIAL_PORT_2, SERIAL_PORT_2_RX_QUEUE_SIZE, SERIAL_PORT_2_TX_QUEUE_SIZE, "2", "2 - WiFi"},
  #endif
  #ifdef SERIAL_PORT_3
    {SERIAL_PORT_3, SERIAL_PORT_3_RX_QUEUE_SIZE, SERIAL_PORT_3_TX_QUEUE_SIZE, "3", "3 - UART3"},
  #endif
  #ifdef SERIAL_PORT_4
    {SERIAL_PORT_4, SERIAL_PORT_4_RX_QUEUE_SIZE, SERIAL_PORT_4_TX_QUEUE_SIZE, "4", "4 - UART4"}
  #endif
};

const uint32_t baudrateValues[BAUDRATE_COUNT] = {
  0,     2400,   9600,   19200,   38400,   57600,   115200,   230400,   250000,   500000,   921600,   1000000};

const char * const baudrateNames[BAUDRATE_COUNT] = {
  "OFF", "2400", "9600", "19200", "38400", "57600", "115200", "230400", "250000", "500000", "921600", "1000000"};

void Serial_Init(SERIAL_PORT_INDEX portIndex)
{
  if (!WITHIN(portIndex, ALL_PORTS, SERIAL_PORT_COUNT - 1))
    return;

  if (portIndex == PORT_1 || portIndex == ALL_PORTS)  // if primary or all serial ports, initialize the primary serial port first
  {
    InfoHost_Init(false);  // initialize infoHost when disconnected

    Serial_Config(serialPort[PORT_1].port, serialPort[PORT_1].cacheSizeRX, serialPort[PORT_1].cacheSizeTX, baudrateValues[infoSettings.serial_port[PORT_1]]);
  }

  #ifdef SERIAL_PORT_2
    // if ALL_PORTS, SUP_PORTS or specific supplementary port
    //
    for (SERIAL_PORT_INDEX tmpIndex = PORT_2; tmpIndex < SERIAL_PORT_COUNT; tmpIndex++)
    {
      // the supplementary serial ports should be enabled according to config.ini.
      // Disable the serial port when it is not in use and/or not connected to a device (floating) to
      // avoid to receive and process wrong data due to possible electromagnetic interference (EMI)

      // if serial port is enabled...
      if (infoSettings.serial_port[tmpIndex] > 0 && (portIndex == tmpIndex || portIndex < PORT_1))
        Serial_Config(serialPort[tmpIndex].port, serialPort[tmpIndex].cacheSizeRX, serialPort[tmpIndex].cacheSizeTX,
                      baudrateValues[infoSettings.serial_port[tmpIndex]]);
    }
  #endif
}

void Serial_DeInit(SERIAL_PORT_INDEX portIndex)
{
  if (!WITHIN(portIndex, ALL_PORTS, SERIAL_PORT_COUNT - 1))
    return;

  if (portIndex == PORT_1 || portIndex == ALL_PORTS)  // if primary or all serial ports, deinitialize the primary serial port first
    Serial_DeConfig(serialPort[PORT_1].port);

  #ifdef SERIAL_PORT_2
    // if ALL_PORTS, SUP_PORTS or specific supplementary port
    //
    for (SERIAL_PORT_INDEX tmpIndex = PORT_2; tmpIndex < SERIAL_PORT_COUNT; tmpIndex++)
    {
      if (portIndex == tmpIndex || portIndex < PORT_1)
        Serial_DeConfig(serialPort[tmpIndex].port);
    }
  #endif
}

void Serial_Forward(SERIAL_PORT_INDEX portIndex, const char * msg)
{
  if (!WITHIN(portIndex, ALL_PORTS, SERIAL_PORT_COUNT - 1))
    return;

  #if defined(SERIAL_DEBUG_PORT) && defined(DEBUG_SERIAL_COMM)
    // dump serial data sent to debug port
    Serial_Put(SERIAL_DEBUG_PORT, ">>");
    Serial_Put(SERIAL_DEBUG_PORT, msg);
  #endif

  uint8_t portCount = SERIAL_PORT_COUNT;  // by default, select all the serial ports

  if (portIndex == ALL_PORTS)         // if ALL_PORTS, forward the message to all the enabled serial ports
    portIndex = PORT_1;
  #ifdef SERIAL_PORT_2
    else if (portIndex == SUP_PORTS)  // if SUP_PORTS, forward the message to all the enabled supplementary serial ports
      portIndex = PORT_2;
  #endif
  else                                // if specific port, forward the message only to that specific serial port, if enabled
    portCount = portIndex + 1;

  for (; portIndex < portCount; portIndex++)
  {
    // forward data only if serial port is enabled
    if (infoSettings.serial_port[portIndex] > 0
        #ifdef SERIAL_DEBUG_PORT
          && serialPort[portIndex].port != SERIAL_DEBUG_PORT  // do not forward data to serial debug port
        #endif
        )
      Serial_Put(serialPort[portIndex].port, msg);  // pass on the message to the port
  }
}

bool Serial_DataAvailableRX(uint8_t port)
{
  // NOTE: used 32 bit variables for performance reasons

  // wIndex: update L1 cache's writing index (dynamically changed (by L1 cache's interrupt handler) variables/attributes)
  //         and make a static access (32 bit) to it to speedup performance on this function
  //
  uint32_t wIndex = dmaL1DataRX[port].wIndex = Serial_GetWritingIndexRX(port);  // get the latest wIndex
  uint32_t flag = dmaL1DataRX[port].flag;                                       // get the current flag position

  if (flag == wIndex)  // if no data to read from L1 cache, nothing to do
    return 0;

  uint32_t cacheSize = dmaL1DataRX[port].cacheSize;

  while (dmaL1DataRX[port].cache[flag] != '\n' && flag != wIndex)  // check presence of "\n" in available data
  {
    flag = (flag + 1) % cacheSize;
  }

  dmaL1DataRX[port].flag = flag;  // update queue's custom flag with flag

  // return "true" if "\n" was found (message complete), "False" otherwise
  return (flag != wIndex);
}

uint16_t Serial_Get(uint8_t port, char * buf, uint16_t bufSize)
{
  // NOTE: used 32 bit variables for performance reasons (in particular for data copy)

  // rIndex: L1 cache's reading index (not dynamically changed (by L1 cache's interrupt handler) variables/attributes)
  //
  DMA_CIRCULAR_BUFFER * dmaL1Data_ptr = &dmaL1DataRX[port];
  char * cache = dmaL1Data_ptr->cache;
  uint32_t cacheSize = dmaL1Data_ptr->cacheSize;
  uint32_t rIndex = dmaL1Data_ptr->rIndex;
  uint32_t flag = dmaL1Data_ptr->flag;

  while (cache[rIndex] == ' ' && rIndex != flag)  // remove leading empty space, if any
  {
    rIndex = (rIndex + 1) % cacheSize;
  }

  // msgSize: message size (after updating rIndex removing leading empty spaces). Terminating null character '\0' not included
  uint32_t msgSize = (cacheSize + flag - rIndex) % cacheSize + 1;

  // if buf size is not enough to store the data plus the terminating null character "\0", skip the data copy
  //
  // NOTE: the following check should never be matched if buf has a proper size and there is no reading error.
  //       If so, the check could be commented out just to improve performance. Just keep it to make the code more robust
  //
  if (bufSize < (msgSize + 1))  // +1 is for the terminating null character '\0'
    return 0;

  // if data is one chunk only, retrieve data from upper part of circular cache
  if (rIndex <= flag)
  {
    while (rIndex <= flag)
    {
      *(buf++) = cache[rIndex++];
    }
  }
  else  // data at end and beginning of cache
  {
    while (rIndex < cacheSize)
    {
      *(buf++) = cache[rIndex++];
    }

    rIndex = 0;

    while (rIndex <= flag)
    {
      *(buf++) = cache[rIndex++];
    }
  }

  *buf = '\0';  // end character

  // update queue's custom flag and reading index with next index
  dmaL1Data_ptr->flag = dmaL1Data_ptr->rIndex = (flag + 1) % cacheSize;

  return msgSize;  // return the number of bytes stored in buf
}

#ifdef SERIAL_PORT_2

void Serial_GetFromUART(void)
{
  CMD cmd;

  // scan all the supplementary serial ports
  for (SERIAL_PORT_INDEX portIndex = PORT_2; portIndex < SERIAL_PORT_COUNT; portIndex++)
  {
    // retrieve data only if serial port is enabled
    if (infoSettings.serial_port[portIndex] > 0
        #ifdef SERIAL_DEBUG_PORT
          && serialPort[portIndex].port != SERIAL_DEBUG_PORT  // do not forward data to serial debug port
        #endif
        )
    {
      while (Serial_DataAvailableRX(serialPort[portIndex].port) && Serial_Get(serialPort[portIndex].port, cmd, CMD_MAX_SIZE) != 0)
      {
        handleCmd(cmd, portIndex);
      }
    }
  }
}

#endif
