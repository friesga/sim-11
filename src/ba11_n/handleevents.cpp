#include "ba11_n.h"

void BA11_N::handleEvents()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				running_ = false;
				break;

			default:
				break;
		}
	}
}