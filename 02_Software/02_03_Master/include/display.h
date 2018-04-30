#ifndef _DISPLAY_H
#define _DISPLAY_H

// Include modules
#include <SDL2/SDL.h>
#include <stdio.h>

// Include custom modules
#include "params.h"

int display_init();
int display_update(location_field_t bicycleDirection);
void display_cleanup();

#endif
