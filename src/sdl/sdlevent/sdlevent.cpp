#include "sdlevent.h"

SDLEvent::SDLEvent (SDL_Event* event, Position mouseTexturePosition)
    :
    event_ {event},
    mouseTexturePosition_ {mouseTexturePosition}
{}

SDL_Event* SDLEvent::getSDL_Event () const
{
    return event_;
}

Position SDLEvent::mouseTexturePosition () const
{
    return mouseTexturePosition_;
}
