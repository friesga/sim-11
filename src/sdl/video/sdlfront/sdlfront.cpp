#include "sdlfront.h"

#include <SDL3_image/SDL_image.h>

using std::make_unique;

SDLFront::SDLFront (unique_ptr<SDLTile> frontTile,
    EventHandler eventHandler)
    : 
    frontTile_ {move (frontTile)},
    eventHandler_ {eventHandler}
{}

SDLFront::~SDLFront ()
{}

void SDLFront::render ()
{
    frontTile_->render ();
}

void SDLFront::handleEvent (InputEvent const* event)
{
    if (eventHandler_ && isWithinBounds (event->mousePosition (), 0.0f))
        eventHandler_ (event);
}

bool SDLFront::isWithinBounds (Position position, float margin) const
{
    // Any of the tiles can be used to check the dimensions
    return frontTile_->isWithinBounds (position, margin);
}

