#include "display.h"

// Screen dimension constants
const uint16_t SCREEN_WIDTH = 600;
const uint16_t SCREEN_HEIGHT = 960;

int main() {

  display_init();

  // Wait two seconds
  SDL_Delay(1000);

  display_update(NORTH);
  SDL_Delay(1000);
  display_update(NORTHEAST);
  SDL_Delay(1000);
  display_update(EAST);
  SDL_Delay(1000);
  display_update(SOUTHEAST);
  SDL_Delay(1000);
  display_update(SOUTH);
  SDL_Delay(1000);
  display_update(SOUTHWEST);
  SDL_Delay(1000);
  display_update(WEST);
  SDL_Delay(1000);
  display_update(NORTHWEST);

  SDL_Delay(10000);

  display_destroy();

  return 0;
}
