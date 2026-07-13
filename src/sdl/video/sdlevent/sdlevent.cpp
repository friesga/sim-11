#include "sdlevent.h"

SDLEvent::SDLEvent (SDL_Event* event, Position mouseTexturePosition)
    :
    event_ {event},
    mouseTexturePosition_ {mouseTexturePosition}
{}

InputEvent::Type SDLEvent::type () const
{
    switch (event_->type)
    {
        case SDL_QUIT:
            return InputEvent::Type::Quit;

        case SDL_MOUSEBUTTONDOWN:
            return InputEvent::Type::MouseButtonDown;

        case SDL_MOUSEBUTTONUP:
            return InputEvent::Type::MouseButtonUp;

        case SDL_MOUSEMOTION:
            return InputEvent::Type::MouseMotion;

        default:
            return InputEvent::Type::Other;
    }
}

InputEvent::Button SDLEvent::button () const
{
    return event_->button.button == SDL_BUTTON_LEFT ?
        InputEvent::Button::Left : InputEvent::Button::Right;
}

Position SDLEvent::mousePosition () const
{
    return mouseTexturePosition_;
}

