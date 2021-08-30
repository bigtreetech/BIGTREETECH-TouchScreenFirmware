#include "SerialConnection.h"
#include "includes.h"

#define SERIAL_PORT_QUEUE_SIZE   NOBEYOND(512, RAM_SIZE * 64, 4096)
#define SERIAL_PORT_2_QUEUE_SIZE 512
#define SERIAL_PORT_3_QUEUE_SIZE 512
#define SERIAL_PORT_4_QUEUE_SIZE 512

SERIAL_PORT_INFO serialPort[SERIAL_PORT_COUNT] = {
  {SERIAL_PORT, SERIAL_PORT_QUEUE_SIZE, false, "", "1 - Printer"},
  #ifdef SERIAL_PORT_2
    {SERIAL_PORT_2, SERIAL_PORT_2_QUEUE_SIZE, false, "2", "2 - WIFI"},
  #endif
  #ifdef SERIAL_PORT_3
    {SERIAL_PORT_3, SERIAL_PORT_3_QUEUE_SIZE, false, "3", "3 - UART3"},
  #endif
  #ifdef SERIAL_PORT_4
    {SERIAL_PORT_4, SERIAL_PORT_4_QUEUE_SIZE, false, "4", "4 - UART4"}
  #endif
};

uint8_t serialPortIndex[_UART_CNT] =               {0};
const uint32_t baudrateValues[BAUDRATE_COUNT] =    { 0,     2400,   9600,   19200,   38400,   57600,   115200,   250000,   500000,   1000000};
const char * const baudrateNames[BAUDRATE_COUNT] = {"OFF", "2400", "9600", "19200", "38400", "57600", "115200", "250000", "500000", "1000000"};

static inline void Serial_InitPrimary(void)
{
  infoHost.connected = infoHost.wait = infoHost.printing = false;
  reminderSetUnConnected();  // reset connect status
  reminderMessage(LABEL_UNCONNECTED, STATUS_UNCONNECT);

  Serial_Config(serialPort[0].port, serialPort[0].cacheSize, baudrateValues[infoSettings.serial_port[0]]);
}

static inline void Serial_DeInitPrimary(void)
{
  Serial_DeConfig(serialPort[0].port);
}

void Serial_Init(int8_t port)
{
  if (port <= 0)  // if primary or all serial ports
  {
    Serial_InitPrimary();

    #ifdef SERIAL_PORT_2
      if (port < 0)  // if < 0, initialize also all the supplementary serial ports
      {
        for (uint8_t i = 1; i < SERIAL_PORT_COUNT; i++)
        {
          // the supplementary serial ports should be enabled according to config.ini.
          // Disable the serial port when it is not in use and/or not connected to a device (floating) to
          // avoid to receive and process wrong data due to possible electromagnetic interference (EMI).
          if (infoSettings.serial_port[i] >= 0)  // if serial port is enabled
            Serial_Config(serialPort[i].port, serialPort[i].cacheSize, baudrateValues[infoSettings.serial_port[i]]);

          serialPortIndex[serialPort[i].port] = i;  // 1 for SERIAL_PORT_2 etc...
        }
      }
    #endif
  }
  #ifdef SERIAL_PORT_2
    else if (port < SERIAL_PORT_COUNT)  // if supplementary serial port
    {
      if (infoSettings.serial_port[port] >= 0)  // if serial port is enabled
      {
        Serial_Config(serialPort[port].port, serialPort[port].cacheSize, baudrateValues[infoSettings.serial_port[port]]);
      }
    }
  #endif
}

void Serial_DeInit(int8_t port)
{
  if (port <= 0)  // if primary or all serial ports
  {
    Serial_DeInitPrimary();

    #ifdef SERIAL_PORT_2
      if (port < 0)  // if < 0, deinitialize also all the supplementary serial ports
      {
        for (uint8_t i = 1; i < SERIAL_PORT_COUNT; i++)
        {
          Serial_DeConfig(serialPort[i].port);
        }
      }
    #endif
  }
  #ifdef SERIAL_PORT_2
    else if (port < SERIAL_PORT_COUNT)  // if supplementary serial port
    {
      Serial_DeConfig(serialPort[port].port);
    }
  #endif
}
