#ifndef _CORNER_H
#define _CORNER_H

// Include modules
#include <stdint.h>
#include <time.h>

// Include custom modules
#include "params.h"
#include "uart.h"

static const uint8_t NUM_RSSI_VALUES = 5;
static const uint8_t FRESHNESS_TIME_DIFF = 5;

typedef struct rssi_data_t {
  time_t rssiTimestamp;
  int8_t rssi;
} rssi_data_t;

typedef struct corner_node_t {
  uint8_t vehicleAddress;
  node_t cornerAddress;
  rssi_data_t rssiData[NUM_RSSI_VALUES];
} corner_node_t;

int32_t corner_init();
location_field_t corner_approximatePosition();

#endif
