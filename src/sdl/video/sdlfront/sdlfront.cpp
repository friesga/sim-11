#include "sdlfront.h"

#include <SDL3_image/SDL_image.h>

using std::make_unique;

SDLFront::SDLFront (unique_ptr<SDLTile> frontTile)
    : 
    frontTile_ (move (frontTile))
{}

SDLFront::~SDLFront ()
{}

void SDLFront::render ()
{
    frontTile_->render ();
}

