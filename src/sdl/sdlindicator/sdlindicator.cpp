#include "sdlindicator.h"

#include <SDL_image.h>

using std::make_unique;

SDLIndicator::SDLIndicator (string imageFile, SDL_Renderer *renderer,
    State showIndicator, int x, int y, int width, int height)
        :
    showIndicator_ {showIndicator}
{
    sdlTtexture_ = make_unique<SDLTexture> (imageFile, renderer, x, y,
        width, height);
}

SDLIndicator::~SDLIndicator ()
{}

void SDLIndicator::render ()
{
    if (showIndicator_ == Indicator::State::On)
        sdlTtexture_->render ();
}

void SDLIndicator::show (Indicator::State showIndicator)
{
    showIndicator_ = showIndicator;
}