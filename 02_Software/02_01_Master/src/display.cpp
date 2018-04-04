
#include "display.h"

// Screen dimension constants
const uint16_t SCREEN_WIDTH = 600;
const uint16_t SCREEN_HEIGHT = 960;

// Initialize the SDL objects we want to use
static SDL_Window   *window          = NULL;
static SDL_Renderer *renderer        = NULL;
static SDL_Surface  *vehicle_image   = NULL;
static SDL_Texture  *vehicle_texture = NULL;
static SDL_Surface  *bicycle_image   = NULL;
static SDL_Texture  *bicycle_texture = NULL;

int display_init() {

  int retv = 0;

  // Initialize SDL
  if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
    printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    retv = -1;
  }
  else {
    // Create window
    window = SDL_CreateWindow("iCycle UI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                           SCREEN_WIDTH, SCREEN_HEIGHT,
                                           SDL_WINDOW_SHOWN );
    if(window == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
      retv = -1;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == NULL) {
      printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
      retv = -1;
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    vehicle_image = SDL_LoadBMP("/home/pi/Documents/iCycle-WSN/02_Software/02_01_Master/data/vehicle.bmp");
    if(vehicle_image == NULL) {
      printf("Image could not be created! SDL_Error: %s\n", SDL_GetError());
      retv = -1;
    }

    vehicle_texture = SDL_CreateTextureFromSurface(renderer, vehicle_image);
    if(vehicle_texture == NULL) {
      printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
      retv = -1;
    }

    SDL_Rect vehiclePos = {(SCREEN_WIDTH-vehicle_image->w)/2, (SCREEN_HEIGHT-vehicle_image->h)/2, vehicle_image->w, vehicle_image->h};
    SDL_RenderCopy(renderer, vehicle_texture, NULL, &vehiclePos);
    SDL_RenderPresent(renderer);
  }

  return retv;
}

int display_update(location_field_t bicycleDirection) {

  int retv = 0;

  bicycle_image = SDL_LoadBMP("/home/pi/Documents/iCycle-WSN/02_Software/02_01_Master/data/bicycle.bmp");
  if(bicycle_image == NULL) {
    printf("Image could not be created! SDL_Error: %s\n", SDL_GetError());
    retv = -1;
  }

  bicycle_texture = SDL_CreateTextureFromSurface(renderer, bicycle_image);
  if(bicycle_texture == NULL) {
    printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
    retv = -1;
  }

  uint16_t bicycleX = 0;
  uint16_t bicycleY = 0;

  uint16_t bicycleWidth = (bicycle_image->w)/5;
  uint16_t bicycleHeight = (bicycle_image->h)/5;

  switch (bicycleDirection) {
    case NORTH:
      bicycleX = SCREEN_WIDTH/2-bicycleWidth/2;
      bicycleY = 0;
      break;
    case NORTHEAST:
      bicycleX = SCREEN_WIDTH-bicycleWidth;
      bicycleY = 0;
      break;
    case EAST:
      bicycleX = SCREEN_WIDTH-bicycleWidth;
      bicycleY = SCREEN_HEIGHT/2-bicycleHeight/2;
      break;
    case SOUTHEAST:
      bicycleX = SCREEN_WIDTH-bicycleWidth;
      bicycleY = SCREEN_HEIGHT-bicycleHeight;
      break;
    case SOUTH:
      bicycleX = SCREEN_WIDTH/2-bicycleWidth/2;
      bicycleY = SCREEN_HEIGHT-bicycleHeight;
      break;
    case SOUTHWEST:
      bicycleX = 0;
      bicycleY = SCREEN_HEIGHT-bicycleHeight;
      break;
    case WEST:
      bicycleX = 0;
      bicycleY = SCREEN_HEIGHT/2-bicycleHeight/2;
      break;
    case NORTHWEST:
      bicycleX = 0;
      bicycleY = 0;
      break;
    default:
      printf("Invalid direction given!\n");
      retv = -1;
      break;
  }

  SDL_Rect bicyclePos = {bicycleX, bicycleY, bicycleWidth, bicycleHeight};
  SDL_RenderCopy(renderer, bicycle_texture, NULL, &bicyclePos);
  SDL_RenderPresent(renderer);

  return retv;
}

int display_destroy() {
  // Destroy the SDL objects
  SDL_DestroyWindow(window);
  SDL_DestroyTexture(vehicle_texture);
  SDL_FreeSurface(vehicle_image);
  SDL_DestroyTexture(bicycle_texture);
  SDL_FreeSurface(bicycle_image);
  SDL_DestroyRenderer(renderer);

  // Quit SDL subsystems
  SDL_Quit();

  return 0;
}
