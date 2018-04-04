#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <SDL2/SDL.h>
#include <stdio.h>

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

int display_init();
int display_update(location_field_t bicycleDirection);
int display_destroy();

#endif
