#include "sdllatchingbutton.h"

using std::make_unique;

SDLLatchingButton::SDLLatchingButton (string buttonDownImage, string buttonUpImage,
    Button::State initialState, unique_ptr<SDLRenderer> &sdlRenderer,
    EventCallback buttonClicked, 
    SDL_Texture* targetTexture, Panel::BoundingBox boundingBox)
    :
    SDLButton (buttonDownImage, buttonUpImage, initialState, sdlRenderer,
        buttonClicked, targetTexture, boundingBox)
{}

SDLLatchingButton::~SDLLatchingButton ()
{}


void SDLLatchingButton::handleEvent (SDLEvent const *event)
{
    SDL_Event* sdlEvent = event->getSDL_Event ();

    if (sdlEvent->type == SDL_MOUSEBUTTONDOWN &&
        sdlEvent->button.button == SDL_BUTTON_LEFT &&
        isWithinBounds (event->mouseTexturePosition ()))
    {
        buttonState_ = toggleState (buttonState_);
        buttonClicked_ (buttonState_);
    }
}