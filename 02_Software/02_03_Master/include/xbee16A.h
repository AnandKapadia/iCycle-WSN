// Import modules
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static const uint16_t MAX_XBEE16A_DATA_SIZE = 100;
static const uint16_t MAX_BUFFER_LENGTH = 1024;

typedef enum xbee16A_result_t {
  XBEE16A_SUCCESS,
  XBEE16A_FAILURE,
} xbee16A_result_t;

typedef struct xbee16A_nodePacket_t {
  uint8_t orientation;
  uint8_t xAcceleration;
  uint8_t yAcceleration;
  uint8_t zAcceleration;
} xbee16A_nodePacket_t;

// Packet formats come from: https://www.sparkfun.com/datasheets/Wireless/Zigbee/XBee-Datasheet.pdf
typedef struct xbee16A_rxCommandData_t {
  uint16_t sourceAddress;
  uint8_t rssi;
  uint8_t options;
  uint8_t data[MAX_XBEE16A_DATA_SIZE];
} xbee16A_rxCommandData_t;

typedef struct xbee16A_rxFrameData_t {
  uint8_t apiIdentifier;
  xbee16A_rxCommandData_t commandData;
} xbee16A_rxFrameData_t;

typedef struct xbee16A_rxPacket_t {
  uint8_t startDelimiter;
  uint16_t length;
  xbee16A_rxFrameData_t frameData;
  uint8_t checksum;
} xbee16A_rxPacket_t;

typedef struct xbee16A_txCommandData_t {
  uint8_t frameId;
  uint16_t destinationAddress;
  uint8_t options;
  uint8_t data[MAX_XBEE16A_DATA_SIZE];
} xbee16A_txCommandData_t;

typedef struct xbee16A_txFrameData_t {
  uint8_t apiIdentifier;
  xbee16A_txCommandData_t commandData;
} xbee16A_txFrameData_t;

typedef struct xbee16A_txPacket_t {
  uint8_t startDelimiter;
  uint16_t length;
  xbee16A_txFrameData_t frameData;
} xbee16A_txPacket_t;

xbee16A_result_t xbee16A_init();
xbee16A_result_t xbee16A_write(xbee16A_txPacket_t *command);
xbee16A_result_t xbee16A_read(xbee16A_rxPacket_t *response);
void xbee16A_readResponseDataOut(xbee16A_rxPacket_t *response, xbee16A_nodePacket_t *nodePacket);
