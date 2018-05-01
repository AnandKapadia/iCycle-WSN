
// Import custom modules
#include "display.h"

//------------------------------------------------------------------------------
// GLOBAL CONSTANT DEFINITIONS                                                 |
//------------------------------------------------------------------------------
const uint16_t SCREEN_WIDTH = 600;
const uint16_t SCREEN_HEIGHT = 960;

//------------------------------------------------------------------------------
// STATIC VARIABLE DEFINITIONS                                                 |
//------------------------------------------------------------------------------
static SDL_Window   *sWindow         = NULL;
static SDL_Renderer *sRenderer       = NULL;
static SDL_Surface  *sVehicleImage   = NULL;
static SDL_Texture  *sVehicleTexture = NULL;
static SDL_Surface  *sBicycleImage   = NULL;
static SDL_Texture  *sBicycleTexture = NULL;

//------------------------------------------------------------------------------
// STATIC HELPER FUNCTIONS                                                     |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
static SDL_Surface *display_loadSurface(const char *filename) {

  SDL_Surface *loadedSurface = NULL;

  loadedSurface = SDL_LoadBMP(filename);
  if(loadedSurface == NULL) {
    printf("Surface could not be created. SDL_Error: %s\n", SDL_GetError());
  }

  return loadedSurface;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------

static SDL_Texture *display_loadTexture(SDL_Surface *loadedSurface) {

  SDL_Texture *loadedTexture = NULL;

  if(loadedSurface == NULL) {
    printf("Invalid surface, cannot load.\n");
  }
  else {
    loadedTexture = SDL_CreateTextureFromSurface(sRenderer, loadedSurface);
    if(loadedTexture == NULL) {
      printf("Texture could not be created. SDL_Error: %s\n", SDL_GetError());
    }
  }

  return loadedTexture;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
static void display_clearRenderer(uint32_t color) {

  uint8_t red   = (color & 0xFF000000) >> 24;
  uint8_t green = (color & 0x00FF0000) >> 16;
  uint8_t blue  = (color & 0x0000FF00) >>  8;
  uint8_t alpha = (color & 0x000000FF) >>  0;

  SDL_SetRenderDrawColor(sRenderer, red, green, blue, alpha);
  SDL_RenderClear(sRenderer);
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------

static void display_addTexture(SDL_Texture *texture, SDL_Rect *rect) { 
  SDL_RenderCopy(sRenderer, texture, NULL, rect);
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------

static void display_render() {
  SDL_RenderPresent(sRenderer);
}

//------------------------------------------------------------------------------
// GLOBAL INTERFACE FUNCTIONS                                                  |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
int display_init() {

  int retv = 0;

  // Initialize SDL
  if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
    printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    retv = -1;
  }
  else {
    // Create window
    sWindow = SDL_CreateWindow("iCycle UI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                           SCREEN_WIDTH, SCREEN_HEIGHT,
                                           SDL_WINDOW_SHOWN );
    if(sWindow == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
      retv = -1;
    }
    sRenderer = SDL_CreateRenderer(sWindow, -1, 0);
    if(sRenderer == NULL) {
      printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
      retv = -1;
    }
    sVehicleImage = display_loadSurface("/home/pi/Documents/iCycle-WSN/02_Software/02_03_Master/data/vehicle.bmp");
    if(sVehicleImage == NULL) {
      printf("Vehicle image could not be created. SDL_Error: %s\n", SDL_GetError());
      retv = -1;
    }
    sVehicleTexture = display_loadTexture(sVehicleImage);
    if(sVehicleImage == NULL) {
      printf("Vehicle image could not be texturized. SDL_Error: %s\n", SDL_GetError());
      retv = -1;
    }
    sBicycleImage = display_loadSurface("/home/pi/Documents/iCycle-WSN/02_Software/02_03_Master/data/bicycle.bmp");
    if(sBicycleImage == NULL) {
      printf("Bicycle image could not be created. SDL_Error: %s\n", SDL_GetError());
      retv = -1;
    }
    sBicycleTexture = display_loadTexture(sBicycleImage);
    if(sBicycleTexture == NULL) {
      printf("Bicycle image could not be textureized. SDL_Error: %s\n", SDL_GetError());
      retv = -1;
    }

    SDL_Rect vehiclePos = {(SCREEN_WIDTH-sVehicleImage->w)/2, (SCREEN_HEIGHT-sVehicleImage->h)/2, sVehicleImage->w, sVehicleImage->h};

    display_clearRenderer(0xFFFFFFFF);
    display_addTexture(sVehicleTexture, &vehiclePos);
    display_render();
  }

  return retv;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
int display_update(location_field_t bicycleDirection) {

  int retv = 0;

  uint16_t bicycleX = 0;
  uint16_t bicycleY = 0;

  uint16_t bicycleWidth = (sBicycleImage->w)/5;
  uint16_t bicycleHeight = (sBicycleImage->h)/5;

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

  SDL_Rect vehiclePos = {(SCREEN_WIDTH-sVehicleImage->w)/2, (SCREEN_HEIGHT-sVehicleImage->h)/2, sVehicleImage->w, sVehicleImage->h};
  SDL_Rect bicyclePos = {bicycleX, bicycleY, bicycleWidth, bicycleHeight};

  display_clearRenderer(0xFFFFFFFF);
  display_addTexture(sVehicleTexture, &vehiclePos);
  display_addTexture(sBicycleTexture, &bicyclePos);
  display_render();

  return retv;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
void display_cleanup() {
  // Destroy the SDL objects
  SDL_DestroyWindow(sWindow);
  SDL_DestroyTexture(sVehicleTexture);
  SDL_FreeSurface(sVehicleImage);
  SDL_DestroyTexture(sBicycleTexture);
  SDL_FreeSurface(sBicycleImage);
  SDL_DestroyRenderer(sRenderer);

  // Quit SDL subsystems
  SDL_Quit();
}
