// Import modules
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Import custom modules
#include "params.h"

//static const uint16_t MAX_UART_DATA_LENGTH = 10;
//static const uint16_t MAX_UART_BUFFER_LENGTH = 10 + MAX_UART_DATA_LENGTH;

typedef enum uart_result_t {
  UART_SUCCESS,
  UART_TIMEOUT,
  UART_FAILURE,
} uart_result_t;

typedef struct uart_address_t {
  vehicle_t vehicleType;
  uint8_t vehicleAddress;
  node_t cornerAddress;
} uart_address_t;

typedef struct uart_rxPacket_t {
  uint8_t packetHeader;
  uart_address_t sourceAddress;
  uint8_t rssi;
  bikeMessage_t bikeMessage;
  uint8_t checksum;
} uart_rxPacket_t;

typedef struct uart_txPacket_t {
  uint8_t packetHeader;
  uart_address_t destinationAddress;
  location_field_t bicycleRelativeToVehicleLocation;
} uart_txPacket_t;

uart_result_t uart_init();
uart_result_t uart_write(uart_txPacket_t *command);
uart_result_t uart_read(uart_rxPacket_t *response);
uart_result_t uart_cleanup();
