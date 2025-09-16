#include "sdlmomentarybutton.h"

SDLMomentaryButton::SDLMomentaryButton (unique_ptr<SDLTexture> buttonDownTexture,
    unique_ptr<SDLTexture> buttonUpTexture,
    Button::TwoPositionsState initialState, EventCallback buttonClicked)
    :
    SDLCommonButton (move (buttonDownTexture), move (buttonUpTexture),
        initialState, buttonClicked),
    naturalState_ {initialState}
{}

SDLMomentaryButton::~SDLMomentaryButton ()
{}

bool SDLMomentaryButton::validMouseEvent (InputEvent const *event)
{
    return (event->type () == InputEvent::Type::MouseButtonDown ||
        event->type () == InputEvent::Type::MouseButtonUp) &&
        event->button () == InputEvent::Button::Left &&
        isWithinBounds (event->mousePosition ());
}

void SDLMomentaryButton::handleEvent (InputEvent const *event)
{
    if (validMouseEvent (event))
    {
        switch (event->type ())
        {
            case InputEvent::Type::MouseButtonDown:
                buttonState_ = toggleState (naturalState_);
                break;

            case InputEvent::Type::MouseButtonUp:
                buttonState_ = naturalState_;
                break;

            default:
                // Ignore all other events
                break;
        }

        buttonClicked_ (buttonState_);
    }
}

