#ifndef _CORNER_H
#define _CORNER_H

// Include modules
#include <stdint.h>
#include <time.h>

// Include custom modules
#include "params.h"

typedef struct corner_node_t {
  uint8_t vehicleAddress;
  node_t cornerAddress;
  time_t rssiTimestamp;
  uint8_t rssi;
} corner_node_t;

int32_t corner_init();
uint8_t getCornerData(uint8_t addr);
location_field_t corner_approximatePosition();

#endif
