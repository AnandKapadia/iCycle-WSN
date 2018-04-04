#include <SDL2/SDL.h>
#include <stdio.h>

// Screen dimension constants
const uint16_t SCREEN_WIDTH = 600;
const uint16_t SCREEN_HEIGHT = 1160;

int main() {

  // Initialize the SDL objects we want to use
  SDL_Window   *window     = NULL;
  SDL_Renderer *renderer   = NULL;
  SDL_Surface  *image      = NULL;
  SDL_Texture  *texture    = NULL;

  // Initialize SDL
  if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
    printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
  }
  else {
    // Create window
    window = SDL_CreateWindow("iCycle UI", SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           SCREEN_WIDTH,
                                           SCREEN_HEIGHT,
                                           SDL_WINDOW_SHOWN );
    if(window == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == NULL) {
      printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
//    char *dataPath = NULL;
//    dataPath = strcat(SDL_GetBasePath(), "../data/vehicle.bmp");
//    printf("PATH: %s\n", dataPath);
    image = SDL_LoadBMP("/home/pi/Documents/iCycle-WSN/02_Software/02_01_Master/data/vehicle.bmp");
    if(image == NULL) {
      printf("Image could not be created! SDL_Error: %s\n", SDL_GetError());
    }
    texture = SDL_CreateTextureFromSurface(renderer, image);
    if(texture == NULL) {
      printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
    }
    SDL_Rect dstrect = {(SCREEN_WIDTH-image->w)/2, (SCREEN_HEIGHT-image->h)/2, image->w, image->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_RenderPresent(renderer);

    // Wait two seconds
    SDL_Delay( 10000 );
  }

  // Destroy the SDL objects
  SDL_DestroyWindow(window);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(image);
  SDL_DestroyRenderer(renderer);

  // Quit SDL subsystems
  SDL_Quit();

  return 0;
}
