#include "ba11_n.h"

void BA11_N::render()
{
	// Clear the renderer to the draw color
	SDL_RenderClear (renderer_);

	SDL_RenderCopy (renderer_, texture_, &sourceRectangle_, 
		&destinationRectangle_);

	// Draw to the screen
	SDL_RenderPresent (renderer_); 
}