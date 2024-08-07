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


void SDLLatchingButton::handleEvent (SDL_Event const *event)
{
    if (event->type == SDL_MOUSEBUTTONDOWN &&
        event->button.button == SDL_BUTTON_LEFT &&
        buttonDownTexture_->isWithinBounds (event->motion.x, event->motion.y))
    {
        buttonState_ = toggleState (buttonState_);
        buttonClicked_ (buttonState_);
    }
}