#include "sdlindicator.h"

#include <SDL_image.h>

using std::make_unique;
using std::move;

SDLIndicator::SDLIndicator (unique_ptr<SDLTile> indicatorOffTile,
    unique_ptr<SDLTile> indicatorOnTile, State showIndicator)
    :
    showIndicator_ {showIndicator},
    indicatorOffTile_ {move (indicatorOffTile)},
    indicatorOnTile_ {move (indicatorOnTile)}
{}

SDLIndicator::~SDLIndicator ()
{}

void SDLIndicator::render ()
{
    if (showIndicator_ == Indicator::State::On)
        indicatorOnTile_->render ();
    else
        indicatorOffTile_->render ();
}

void SDLIndicator::show (Indicator::State showIndicator)
{
    showIndicator_ = showIndicator;
}

bool SDLIndicator::isWithinBounds (Position position, float margin) const
{
    return indicatorOnTile_->isWithinBounds (position, margin);
}