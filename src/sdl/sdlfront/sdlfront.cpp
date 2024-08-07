#include "sdlfront.h"

#include <SDL_image.h>

using std::make_unique;

SDLFront::SDLFront (string imageFile, unique_ptr<SDLRenderer> &sdlRenderer,
    SDL_Texture* targetTexture, Panel::BoundingBox boundingBox)
    : 
    sdlRenderer_ {sdlRenderer}
{
    sdlTtexture_ = make_unique<SDLTexture> (imageFile,
        sdlRenderer->getSDL_Renderer (), targetTexture, boundingBox);
}

SDLFront::~SDLFront ()
{}

void SDLFront::render ()
{
    sdlTtexture_->render ();
}

