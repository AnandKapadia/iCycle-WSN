#ifndef _UART_H_
#define _UART_H_

// Import modules
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Import custom modules
#include "params.h"
#include "msg_structs.h"

static const uint8_t EXPECTED_PACKET_HEADER = 0x7A;

typedef enum uart_result_t {
  UART_SUCCESS,
  UART_TIMEOUT,
  UART_FAILURE,
} uart_result_t;

typedef struct uart_address_t {
  uint8_t vehicleType;
  uint8_t vehicleAddress;
  uint8_t cornerAddress;
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
  location_field_t bicycleRelativeToVehicleOrientation;
  uint8_t checksum;
} uart_txPacket_t;

uart_result_t uart_init();
uart_result_t uart_write(uart_txPacket_t *command);
uart_result_t uart_read(uart_rxPacket_t *response);
uart_result_t uart_cleanup();

#endif

