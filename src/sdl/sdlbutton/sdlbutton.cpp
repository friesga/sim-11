#include "sdlbutton.h"

using std::make_unique;

SDLButton::SDLButton (string buttonDownImage, string buttonUpImage,
    Button::State initialState, SDL_Renderer *renderer,
    EventCallback buttonClicked, int x, int y, int width, int height)
    :
    buttonClicked_ {buttonClicked},
    buttonState_ {initialState}
{
    buttonDownTexture_ = make_unique<SDLTexture> (buttonDownImage, renderer,
        x, y, width, height);
    buttonUpTexture_   = make_unique<SDLTexture> (buttonUpImage, renderer,
        x, y, width, height);
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

