#ifndef _PARAMS_H
#define _PARAMS_H

static const uint8_t NUM_CORNERS = 4;
static const uint8_t MY_VEHICLE_ADDRESS = 0;

typedef enum location_field_t {
  NORTH,
  NORTHEAST,
  EAST,
  SOUTHEAST,
  SOUTH,
  SOUTHWEST,
  WEST,
  NORTHWEST
} location_field_t;

typedef enum uart_vehicle_t {
  BICYCLE,
  AUTOMOBILE,
} vehicle_t;

typedef enum uart_corner_t {
  MASTER,
  FRONT_LEFT,
  FRONT_RIGHT,
  REAR_LEFT,
  REAR_RIGHT,
} node_t;

#endif

