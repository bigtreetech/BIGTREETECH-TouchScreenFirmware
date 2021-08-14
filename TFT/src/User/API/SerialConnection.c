#include "SerialConnection.h"
#include "includes.h"

#define SERIAL_PORT_QUEUE_SIZE   NOBEYOND(512, RAM_SIZE * 64, 4096)
#define SERIAL_PORT_2_QUEUE_SIZE 512
#define SERIAL_PORT_3_QUEUE_SIZE 512
#define SERIAL_PORT_4_QUEUE_SIZE 512

typedef struct
{
  uint8_t port;
  uint16_t cacheSize;
} SERIAL_PORT_INFO;

#ifdef SERIAL_PORT_2
  const SERIAL_PORT_INFO mulSerialPort[] = {
    {SERIAL_PORT_2, SERIAL_PORT_2_QUEUE_SIZE},
    #ifdef SERIAL_PORT_3
      {SERIAL_PORT_3, SERIAL_PORT_3_QUEUE_SIZE},
    #endif
    #ifdef SERIAL_PORT_4
      {SERIAL_PORT_4, SERIAL_PORT_4_QUEUE_SIZE}
    #endif
  };
#endif

const uint32_t baudrateList[BAUDRATE_COUNT] =      { 2400,   9600,   19200,   38400,   57600,   115200,   250000,   500000,   1000000};
const char * const baudrateNames[BAUDRATE_COUNT] = {"2400", "9600", "19200", "38400", "57600", "115200", "250000", "500000", "1000000"};
bool serialHasBeenInitialized = false;

void Serial_ReSourceInit(void)
{
  if (serialHasBeenInitialized)
    return;

  memset(&infoHost, 0, sizeof(infoHost));
  reminderSetUnConnected();  // reset connect status

  Serial_Config(SERIAL_PORT, SERIAL_PORT_QUEUE_SIZE, baudrateList[infoSettings.baudrate]);

  #ifdef SERIAL_PORT_2
    for (uint8_t i = 0; i < sizeof(mulSerialPort); i++)
    {
      // The extended Multi-Serials port should be enabled according to config.ini.
      // Disable the serial port when it is not in use and floating to avoid to receive
      // wrong data due to electromagnetic interference (EMI).
      if (infoSettings.multi_serial & (1 << i))
        Serial_Config(mulSerialPort[i].port, mulSerialPort[i].cacheSize, baudrateList[infoSettings.baudrate]);
    }
  #endif

  serialHasBeenInitialized = true;
}

void Serial_ReSourceDeInit(void)
{
  if (!serialHasBeenInitialized)
    return;

  Serial_DeConfig(SERIAL_PORT);

  #ifdef SERIAL_PORT_2
    for (uint8_t i = 0; i < sizeof(mulSerialPort); i++)
    {
      if (infoSettings.multi_serial & (1 << i))
        Serial_DeConfig(mulSerialPort[i].port);
    }
  #endif

  serialHasBeenInitialized = false;
}
