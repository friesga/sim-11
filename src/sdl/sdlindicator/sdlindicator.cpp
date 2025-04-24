#include "sdlindicator.h"

#include <SDL_image.h>

using std::make_unique;

SDLIndicator::SDLIndicator (string indicatorOffImage, string indicatorOnImage,
    unique_ptr<SDLRenderer> &sdlRenderer, State showIndicator, 
    SDL_Texture* targetTexture, Frame<int> frame)
        :
    showIndicator_ {showIndicator}
{
    indicatorOffTexture_ = make_unique<SDLTexture> (indicatorOffImage,
        sdlRenderer->getSDL_Renderer (), targetTexture, frame);
    indicatorOnTexture_ = make_unique<SDLTexture> (indicatorOnImage, 
        sdlRenderer->getSDL_Renderer (), targetTexture, frame);
}

SDLIndicator::~SDLIndicator ()
{}

void SDLIndicator::render ()
{
    if (showIndicator_ == Indicator::State::On)
        indicatorOnTexture_->render ();
    else
        indicatorOffTexture_->render ();
}

void SDLIndicator::show (Indicator::State showIndicator)
{
    showIndicator_ = showIndicator;
}

bool SDLIndicator::isWithinBounds (Position position, float margin) const
{
    return indicatorOnTexture_->isWithinBounds (position, margin);
}