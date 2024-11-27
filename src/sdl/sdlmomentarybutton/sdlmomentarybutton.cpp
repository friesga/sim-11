#include "sdlmomentarybutton.h"


using std::make_unique;

SDLMomentaryButton::SDLMomentaryButton (string buttonDownImage, string buttonUpImage,
    Button::TwoPositionsState initialState, unique_ptr<SDLRenderer> &sdlRenderer,
    EventCallback buttonClicked, 
    SDL_Texture* targetTexture, Frame<int> frame)
    :
    SDLCommonButton (buttonDownImage, buttonUpImage, initialState, sdlRenderer,
        buttonClicked, targetTexture, frame),
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

