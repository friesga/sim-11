#include "sdlcommonbutton.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <stdexcept>

using std::make_unique;
using std::get;
using std::logic_error;

SDLCommonButton::SDLCommonButton (unique_ptr<SDLTile> buttonDownTile,
    unique_ptr<SDLTile> buttonUpTile,
    Button::TwoPositionsState initialState, 
    EventCallback buttonClicked)
    :
    buttonDownTile_ {move (buttonDownTile)},
    buttonUpTile_ {move (buttonUpTile)},
    buttonClicked_ {buttonClicked},
    buttonState_ {initialState}
{}

SDLCommonButton::~SDLCommonButton ()
{}

void SDLCommonButton::setState (State newState)
{
    buttonState_ = get<TwoPositionsState> (newState);
}

Button::State SDLCommonButton::currentState () const
{
    return buttonState_;
}

Button::TwoPositionsState SDLCommonButton::toggleState (TwoPositionsState oldState)
{
    switch (oldState)
    {
        case TwoPositionsState::Down:
            return TwoPositionsState::Up;

        case TwoPositionsState::Up:
            return TwoPositionsState::Down;
    }

    // Satisfying the compiler
    throw logic_error ("Cannot happen");
}

void SDLCommonButton::render ()
{
    switch (buttonState_)
    {
        case TwoPositionsState::Down:
            buttonDownTile_->render ();
            break;
        
        case TwoPositionsState::Up:
            buttonUpTile_->render ();
    }
}

bool SDLCommonButton::isWithinBounds (Position position, float margin) const
{
    return buttonUpTile_->isWithinBounds (position, margin);
}


