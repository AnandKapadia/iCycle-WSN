#include "display.h"

int main() {

  display_init();

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

  SDL_Delay(5000);

  display_destroy();

  return 0;
}
