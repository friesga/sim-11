#include "sdlevent.h"

SDLEvent::SDLEvent (SDL_Event* event,
    int mouseTexturePositionX, int mouseTexturePositionY)
    :
    event_ {event},
    mouseTexturePositionX_ {mouseTexturePositionX},
    mouseTexturePositionY_ {mouseTexturePositionY}
{}

SDL_Event* SDLEvent::getSDL_Event () const
{
    return event_;
}

int SDLEvent::mouseTexturePositionX () const
{
    return mouseTexturePositionX_;
}

int SDLEvent::mouseTexturePositionY () const
{
    return mouseTexturePositionY_;
}