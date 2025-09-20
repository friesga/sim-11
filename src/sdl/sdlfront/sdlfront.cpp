#include "sdlfront.h"

#include <SDL_image.h>

using std::make_unique;

SDLFront::SDLFront (unique_ptr<SDLTile> frontTexture)
    : 
    sdlTtexture_ (move (frontTexture))
{}

SDLFront::~SDLFront ()
{}

void SDLFront::render ()
{
    sdlTtexture_->render ();
}

