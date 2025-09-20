#include "sdlfront.h"

#include <SDL_image.h>

using std::make_unique;

SDLFront::SDLFront (unique_ptr<SDLTile> frontTile)
    : 
    sdlTile_ (move (frontTile))
{}

SDLFront::~SDLFront ()
{}

void SDLFront::render ()
{
    sdlTile_->render ();
}

