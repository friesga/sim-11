#include "sdlfront.h"

#include <SDL3_image/SDL_image.h>
#include <stdexcept>

using std::make_unique;
using std::invalid_argument;

SDLFront::SDLFront (unique_ptr<SDLTile> frontTile,
    EventHandler eventHandler)
    :
    SDLFront (std::move (frontTile), nullptr,
        State::Primary, std::move (eventHandler))
{}

SDLFront::SDLFront (unique_ptr<SDLTile> primaryTile,
    unique_ptr<SDLTile> secondaryTile,
    State initialState,
    EventHandler eventHandler)
    :
    frontTiles_ {std::move (primaryTile), std::move (secondaryTile)},
    eventHandler_ {std::move (eventHandler)}
{
    if (!frontTiles_[static_cast<size_t> (State::Primary)])
        throw invalid_argument ("Primary front tile missing");

    show (initialState);
}

SDLFront::~SDLFront ()
{}

// Select the image to be shown if a secondary image is provided.
void SDLFront::show (State state)
{
    if (!frontTiles_[static_cast<size_t> (state)])
        throw std::logic_error ("Front state not available");

    state_ = state;
}

void SDLFront::render ()
{
    frontTiles_[static_cast<size_t> (state_)]->render ();
}

void SDLFront::handleEvent (InputEvent const* event)
{
    if (eventHandler_ && isWithinBounds (event->mousePosition (), 0.0f))
        eventHandler_ (event);
}

bool SDLFront::isWithinBounds (Position position, float margin) const
{
    // Any of the tiles can be used to check the dimensions. The primary tile
    // should always be present
    return frontTiles_[static_cast<size_t> (State::Primary)]->isWithinBounds (position, margin);
}

