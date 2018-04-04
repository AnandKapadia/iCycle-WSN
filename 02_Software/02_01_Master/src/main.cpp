#include <SDL2/SDL.h>
#include <stdio.h>

// Screen dimension constants
const uint16_t SCREEN_WIDTH = 600;
const uint16_t SCREEN_HEIGHT = 1160;

int main() {

  // Initialize the SDL objects we want to use
  SDL_Window   *window          = NULL;
  SDL_Renderer *renderer        = NULL;
  SDL_Surface  *vehicle_image   = NULL;
  SDL_Texture  *vehicle_texture = NULL;
  SDL_Surface  *bicycle_image   = NULL;
  SDL_Texture  *bicycle_texture = NULL;

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

    vehicle_image = SDL_LoadBMP("/home/pi/Documents/iCycle-WSN/02_Software/02_01_Master/data/vehicle.bmp");
    if(vehicle_image == NULL) {
      printf("Image could not be created! SDL_Error: %s\n", SDL_GetError());
    }

    vehicle_texture = SDL_CreateTextureFromSurface(renderer, vehicle_image);
    if(vehicle_texture == NULL) {
      printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
    }

    bicycle_image = SDL_LoadBMP("/home/pi/Documents/iCycle-WSN/02_Software/02_01_Master/data/bicycle.bmp");
    if(bicycle_image == NULL) {
      printf("Image could not be created! SDL_Error: %s\n", SDL_GetError());
    }

    bicycle_texture = SDL_CreateTextureFromSurface(renderer, bicycle_image);
    if(bicycle_texture == NULL) {
      printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
    }

    SDL_Rect vehiclePos = {(SCREEN_WIDTH-vehicle_image->w)/2, (SCREEN_HEIGHT-vehicle_image->h)/2,
                                         vehicle_image->w,                   vehicle_image->h };
    SDL_Rect bicyclePos0 = {0, 0, (bicycle_image->w)/5, (bicycle_image->h)/5};
    SDL_Rect bicyclePos1 = {SCREEN_WIDTH/2-((bicycle_image->w)/5)/2, 0, (bicycle_image->w)/5, (bicycle_image->h)/5};
    SDL_Rect bicyclePos2 = {SCREEN_WIDTH-(bicycle_image->w)/5, 0, (bicycle_image->w)/5, (bicycle_image->h)/5};
    SDL_Rect bicyclePos3 = {0, SCREEN_HEIGHT/2-((bicycle_image->h)/5)/2, (bicycle_image->w)/5, (bicycle_image->h)/5};
    SDL_Rect bicyclePos4 = {SCREEN_WIDTH-(bicycle_image->w)/5, SCREEN_HEIGHT/2-((bicycle_image->h)/5)/2, (bicycle_image->w)/5, (bicycle_image->h)/5};
    SDL_Rect bicyclePos5 = {0, SCREEN_HEIGHT-(bicycle_image->h)/5, (bicycle_image->w)/5, (bicycle_image->h)/5};
    SDL_Rect bicyclePos6 = {SCREEN_WIDTH/2-((bicycle_image->w)/5)/2, SCREEN_HEIGHT-(bicycle_image->h)/5, (bicycle_image->w)/5, (bicycle_image->h)/5};
    SDL_Rect bicyclePos7 = {SCREEN_WIDTH-(bicycle_image->w)/5, SCREEN_HEIGHT-(bicycle_image->h)/5, (bicycle_image->w)/5, (bicycle_image->h)/5};
    SDL_RenderCopy(renderer, vehicle_texture, NULL, &vehiclePos);
    SDL_RenderCopy(renderer, bicycle_texture, NULL, &bicyclePos0);
    SDL_RenderCopy(renderer, bicycle_texture, NULL, &bicyclePos1);
    SDL_RenderCopy(renderer, bicycle_texture, NULL, &bicyclePos2);
    SDL_RenderCopy(renderer, bicycle_texture, NULL, &bicyclePos3);
    SDL_RenderCopy(renderer, bicycle_texture, NULL, &bicyclePos4);
    SDL_RenderCopy(renderer, bicycle_texture, NULL, &bicyclePos5);
    SDL_RenderCopy(renderer, bicycle_texture, NULL, &bicyclePos6);
    SDL_RenderCopy(renderer, bicycle_texture, NULL, &bicyclePos7);
    SDL_RenderPresent(renderer);

    // Wait two seconds
    SDL_Delay(10000);
  }

  // Destroy the SDL objects
  SDL_DestroyWindow(window);
  SDL_DestroyTexture(vehicle_texture);
  SDL_FreeSurface(vehicle_image);
  SDL_DestroyRenderer(renderer);

  // Quit SDL subsystems
  SDL_Quit();

  return 0;
}
