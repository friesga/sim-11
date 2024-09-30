#include "sdllatchingbutton.h"

using std::make_unique;

SDLLatchingButton::SDLLatchingButton (string buttonDownImage, string buttonUpImage,
    Button::State initialState, unique_ptr<SDLRenderer> &sdlRenderer,
    EventCallback buttonClicked, 
    SDL_Texture* targetTexture, Frame<int> frame)
    :
    SDLCommonButton (buttonDownImage, buttonUpImage, initialState, sdlRenderer,
        buttonClicked, targetTexture, frame)
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