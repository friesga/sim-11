#include "sdlfront.h"

#include <SDL_image.h>

using std::make_unique;

SDLFront::SDLFront (string imageFile, unique_ptr<SDLRenderer> &sdlRenderer,
    SDL_Texture* targetTexture, Frame<int> frame)
{
    sdlTtexture_ = make_unique<SDLTexture> (imageFile,
        sdlRenderer->getSDL_Renderer (), targetTexture, frame);
}

// This version of the constructor creates an SDLFront from an existing
// SDLTexture.
SDLFront::SDLFront (unique_ptr<SDLTexture> frontTexture)
    : 
    sdlTtexture_ (move (frontTexture))
{}

SDLFront::~SDLFront ()
{}

void SDLFront::render ()
{
    sdlTtexture_->render ();
}

