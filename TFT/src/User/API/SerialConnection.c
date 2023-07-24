#include "SerialConnection.h"
#include "includes.h"

#define SERIAL_PORT_QUEUE_SIZE   NOBEYOND(512, RAM_SIZE * 64, 4096)
#define SERIAL_PORT_2_QUEUE_SIZE 512
#define SERIAL_PORT_3_QUEUE_SIZE 512
#define SERIAL_PORT_4_QUEUE_SIZE 512

const SERIAL_PORT_INFO serialPort[SERIAL_PORT_COUNT] = {
  {SERIAL_PORT, SERIAL_PORT_QUEUE_SIZE, "", "1 - Printer"},
  #ifdef SERIAL_PORT_2
    {SERIAL_PORT_2, SERIAL_PORT_2_QUEUE_SIZE, "2", "2 - WIFI"},
  #endif
  #ifdef SERIAL_PORT_3
    {SERIAL_PORT_3, SERIAL_PORT_3_QUEUE_SIZE, "3", "3 - UART3"},
  #endif
  #ifdef SERIAL_PORT_4
    {SERIAL_PORT_4, SERIAL_PORT_4_QUEUE_SIZE, "4", "4 - UART4"}
  #endif
};

const uint32_t baudrateValues[BAUDRATE_COUNT] =    { 0,     2400,   9600,   19200,   38400,   57600,   115200,   250000,   500000,   1000000};
const char * const baudrateNames[BAUDRATE_COUNT] = {"OFF", "2400", "9600", "19200", "38400", "57600", "115200", "250000", "500000", "1000000"};

void Serial_Init(SERIAL_PORT_INDEX portIndex)
{
  if (!WITHIN(portIndex, ALL_PORTS, SERIAL_PORT_COUNT - 1))
    return;

  if (portIndex == PORT_1 || portIndex == ALL_PORTS)
  {
    Serial_Config(serialPort[PORT_1].port, serialPort[PORT_1].cacheSize, baudrateValues[infoSettings.serial_port[PORT_1]]);

    InfoHost_Init(false);  // initialize infoHost when disconnected
    coordinateSetKnown(false);
  }

  #ifdef SERIAL_PORT_2
    for (SERIAL_PORT_INDEX tmpIndex = PORT_2; tmpIndex < SERIAL_PORT_COUNT; tmpIndex++)
    {
      // the supplementary serial ports should be enabled according to config.ini.
      // Disable the serial port when it is not in use and/or not connected to a device (floating) to
      // avoid to receive and process wrong data due to possible electromagnetic interference (EMI).

      if (infoSettings.serial_port[tmpIndex] == 0)  // if serial port not enabled, skip
        continue;

      if (portIndex == tmpIndex || portIndex < PORT_1)
        Serial_Config(serialPort[tmpIndex].port, serialPort[tmpIndex].cacheSize,
                            baudrateValues[infoSettings.serial_port[tmpIndex]]);
    }
  #endif
}

void Serial_DeInit(SERIAL_PORT_INDEX portIndex)
{
  if (!WITHIN(portIndex, ALL_PORTS, SERIAL_PORT_COUNT - 1))
    return;

  if (portIndex == PORT_1 || portIndex == ALL_PORTS)
    Serial_DeConfig(serialPort[PORT_1].port);

  #ifdef SERIAL_PORT_2
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

uint16_t Serial_GetReadingIndex(SERIAL_PORT_INDEX portIndex)
{
  if (!WITHIN(portIndex, PORT_1, SERIAL_PORT_COUNT - 1))
    return 0;

  return dmaL1Data[portIndex].rIndex;
}

uint16_t Serial_Get(SERIAL_PORT_INDEX portIndex, char * buf, uint16_t bufSize)
{
  // if port index is out of range or no data to read from L1 cache
  if (!WITHIN(portIndex, PORT_1, SERIAL_PORT_COUNT - 1) || dmaL1Data[portIndex].flag == dmaL1Data[portIndex].wIndex)
    return 0;

  DMA_CIRCULAR_BUFFER * dmaL1Data_ptr = &dmaL1Data[portIndex];

  // make a static access to dynamically changed (by L1 cache's interrupt handler) variables/attributes
  uint16_t wIndex = dmaL1Data_ptr->wIndex;

  // L1 cache's reading index (not dynamically changed (by L1 cache's interrupt handler) variables/attributes)
  uint16_t rIndex = dmaL1Data_ptr->rIndex;

  while (dmaL1Data_ptr->cache[rIndex] == ' ' && rIndex != wIndex)  // remove leading empty space, if any
  {
    rIndex = (rIndex + 1) % dmaL1Data_ptr->cacheSize;
  }

  for (uint16_t i = 0; i < (bufSize - 1) && rIndex != wIndex; )  // retrieve data until buf is full or L1 cache is empty
  {
    buf[i] = dmaL1Data_ptr->cache[rIndex];
    rIndex = (rIndex + 1) % dmaL1Data_ptr->cacheSize;

    if (buf[i++] == '\n')  // if data end marker is found
    {
      buf[i] = '\0';                                         // end character
      dmaL1Data_ptr->flag = dmaL1Data_ptr->rIndex = rIndex;  // update queue's custom flag and reading index with rIndex

      return i;  // return the number of bytes stored in buf
    }
  }

  // if here, a partial message is present on the L1 cache (message not terminated by "\n").
  // We temporary skip the message until it is fully received updating also dmaL1Data_ptr->flag to
  // prevent to read again (multiple times) the same partial message on next function invokation

  dmaL1Data_ptr->flag = wIndex;  // update queue's custom flag with wIndex

  return 0;  // return the number of bytes stored in buf
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
      while (Serial_Get(serialPort[portIndex].port, cmd, CMD_MAX_SIZE) != 0)  // if some data have been retrieved
      {
        handleCmd(cmd, portIndex);
      }
    }
  }
}

#endif
