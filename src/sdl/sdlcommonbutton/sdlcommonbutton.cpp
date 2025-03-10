#include "sdlcommonbutton.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <stdexcept>

using std::make_unique;
using std::get;
using std::logic_error;

SDLCommonButton::SDLCommonButton (string buttonDownImage, string buttonUpImage,
    Button::TwoPositionsState initialState, unique_ptr<SDLRenderer> &sdlRenderer,
    EventCallback buttonClicked, 
    SDL_Texture* targetTexture, Frame<int> frame)
    :
    buttonClicked_ {buttonClicked},
    buttonState_ {initialState}
{
    buttonDownTexture_ = make_unique<SDLTexture> (buttonDownImage, 
        sdlRenderer->getSDL_Renderer(), targetTexture, frame);
    buttonUpTexture_   = make_unique<SDLTexture> (buttonUpImage, 
        sdlRenderer->getSDL_Renderer (), targetTexture, frame);
}

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
            buttonDownTexture_->render ();
            break;
        
        case TwoPositionsState::Up:
            buttonUpTexture_->render ();
    }
}

bool SDLCommonButton::isWithinBounds (Position position, float margin) const
{
    return buttonUpTexture_->isWithinBounds (position, margin);
}


