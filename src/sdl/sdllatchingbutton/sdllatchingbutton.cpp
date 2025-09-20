#include "sdllatchingbutton.h"

using std::make_unique;
using std::move;

SDLLatchingButton::SDLLatchingButton (unique_ptr<SDLTile> buttonDownTile,
    unique_ptr<SDLTile> buttonUpTile,
    Button::TwoPositionsState initialState,
    EventCallback buttonClicked)
    :
    SDLCommonButton (move (buttonDownTile), move (buttonUpTile),
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