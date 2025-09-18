#include "sdlindicator.h"

#include <SDL_image.h>

using std::make_unique;
using std::move;

SDLIndicator::SDLIndicator (unique_ptr<SDLTexture> indicatorOffTexture,
    unique_ptr<SDLTexture> indicatorOnTexture, State showIndicator)
    :
    showIndicator_ {showIndicator},
    indicatorOffTexture_ {move (indicatorOffTexture)},
    indicatorOnTexture_ {move (indicatorOnTexture)}
{}

SDLIndicator::~SDLIndicator ()
{}

void SDLIndicator::render (GraphicsContext& context)
{
    if (showIndicator_ == Indicator::State::On)
        indicatorOnTexture_->render (context);
    else
        indicatorOffTexture_->render (context);
}

void SDLIndicator::show (Indicator::State showIndicator)
{
    showIndicator_ = showIndicator;
}

bool SDLIndicator::isWithinBounds (Position position, float margin) const
{
    return indicatorOnTexture_->isWithinBounds (position, margin);
}