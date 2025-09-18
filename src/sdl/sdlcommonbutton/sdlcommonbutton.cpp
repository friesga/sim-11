#include "sdlcommonbutton.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <stdexcept>

using std::make_unique;
using std::get;
using std::logic_error;

SDLCommonButton::SDLCommonButton (unique_ptr<SDLTexture> buttonDownTexture,
    unique_ptr<SDLTexture> buttonUpTexture,
    Button::TwoPositionsState initialState, 
    EventCallback buttonClicked)
    :
    buttonDownTexture_ {move (buttonDownTexture)},
    buttonUpTexture_ {move (buttonUpTexture)},
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
        case TwoPositionsState::On:
            return TwoPositionsState::Off;

        case TwoPositionsState::Off:
            return TwoPositionsState::On;
    }

    // Satisfying the compiler
    throw logic_error ("Cannot happen");
}

void SDLCommonButton::render (GraphicsContext& context)
{
    switch (buttonState_)
    {
        case TwoPositionsState::On:
            buttonDownTexture_->render (context);
            break;
        
        case TwoPositionsState::Off:
            buttonUpTexture_->render (context);
    }
}

bool SDLCommonButton::isWithinBounds (Position position, float margin) const
{
    return buttonUpTexture_->isWithinBounds (position, margin);
}


