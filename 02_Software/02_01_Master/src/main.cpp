//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main() {
	//The window we'll be rendering to
	SDL_Window *window = NULL;
	
	//The surface contained by the window
	SDL_Surface *screenSurface = NULL;

	//Initialize SDL
	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else {
		//Create window
		window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
                                               SDL_WINDOWPOS_UNDEFINED,
                                               SCREEN_WIDTH,
                                               SCREEN_HEIGHT,
                                               SDL_WINDOW_SHOWN );
		if( window == NULL ) {
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		}
		else {
		
      SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
      SDL_Surface *image = SDL_LoadBMP("vehicle.bmp");
      SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);

			//Wait two seconds
			SDL_Delay( 2000 );

      SDL_DestroyTexture(texture);
      SDL_FreeSurface(image);
      SDL_DestroyRenderer(renderer);
		}
	}

	//Destroy window
	SDL_DestroyWindow( window );

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}
