#include "sdlindicator.h"

#include <SDL_image.h>

using std::make_unique;

SDLIndicator::SDLIndicator (string imageFile,
    unique_ptr<SDLRenderer> &sdlRenderer, State showIndicator, 
    SDL_Texture* targetTexture, int x, int y, int width, int height)
        :
    showIndicator_ {showIndicator}
{
    sdlTtexture_ = make_unique<SDLTexture> (imageFile, 
        sdlRenderer->getSDL_Renderer (), targetTexture, x, y, width, height);
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