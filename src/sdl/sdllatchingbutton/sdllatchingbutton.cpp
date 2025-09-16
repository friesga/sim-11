#include "sdllatchingbutton.h"

using std::make_unique;
using std::move;

SDLLatchingButton::SDLLatchingButton (unique_ptr<SDLTexture> buttonDownTexture,
    unique_ptr<SDLTexture> buttonUpTexture,
    Button::TwoPositionsState initialState,
    EventCallback buttonClicked)
    :
    SDLCommonButton (move (buttonDownTexture), move (buttonUpTexture),
        initialState, buttonClicked)
{}

SDLLatchingButton::~SDLLatchingButton ()
{}

void SDLLatchingButton::handleEvent (InputEvent const *event)
{
        if (event->type () == InputEvent::Type::MouseButtonDown &&
        event->button () == InputEvent::Button::Left &&
        isWithinBounds (event->mousePosition ()))
    {
        buttonState_ = toggleState (buttonState_);
        buttonClicked_ (buttonState_);
    }
}