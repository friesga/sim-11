#include "sdlbutton.h"
#include "../sdlrenderer/sdlrenderer.h"

using std::make_unique;

SDLButton::SDLButton (string buttonDownImage, string buttonUpImage,
    Button::State initialState, unique_ptr<SDLRenderer> &sdlRenderer,
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

SDLButton::~SDLButton ()
{}

Button::State SDLButton::toggleState (State oldState)
{
    switch (oldState)
    {
        case State::Down:
            return State::Up;

        case State::Up:
            return State::Down;
    }

    // Satisfying the compiler
    throw string ("Cannot happen");
}

void SDLButton::render ()
{
    switch (buttonState_)
    {
        case State::Down:
            buttonDownTexture_->render ();
            break;
        
        case State::Up:
            buttonUpTexture_->render ();
    }
}

bool SDLButton::isWithinBounds (Position position, float margin) const
{
    return buttonUpTexture_->isWithinBounds (position, margin);
}


