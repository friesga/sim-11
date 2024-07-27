#include "sdlindicator.h"

#include <SDL_image.h>

using std::make_unique;

SDLIndicator::SDLIndicator (string imageFile,
    unique_ptr<SDLRenderer> &sdlRenderer, State showIndicator, 
    int x, int y, int width, int height)
        :
    showIndicator_ {showIndicator}
{
    sdlTtexture_ = make_unique<SDLTexture> (imageFile, 
        sdlRenderer->getSDL_Renderer (), x, y, width, height);
}

SDLIndicator::~SDLIndicator ()
{}

void SDLIndicator::render ()
{
    if (showIndicator_ == Indicator::State::On)
        sdlTtexture_->render ();
}

void SDLIndicator::render (SDL_Texture* texture)
{
    if (showIndicator_ == Indicator::State::On)
        sdlTtexture_->render (texture);
}

void SDLIndicator::show (Indicator::State showIndicator)
{
    showIndicator_ = showIndicator;
}