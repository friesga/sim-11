#include "sdlmomentarybutton.h"


using std::make_unique;

SDLMomentaryButton::SDLMomentaryButton (string buttonDownImage, string buttonUpImage,
    Button::State initialState, unique_ptr<SDLRenderer> &sdlRenderer,
    EventCallback buttonClicked, 
    SDL_Texture* targetTexture, Panel::BoundingBox boundingBox)
    :
    SDLButton (buttonDownImage, buttonUpImage, initialState, sdlRenderer,
        buttonClicked, targetTexture, boundingBox),
    naturalState_ {initialState}
{}

SDLMomentaryButton::~SDLMomentaryButton ()
{}

bool SDLMomentaryButton::validMouseEvent (SDL_Event const *event)
{
    return (event->type == SDL_MOUSEBUTTONDOWN || 
        event->type == SDL_MOUSEBUTTONUP) &&
        event->button.button == SDL_BUTTON_LEFT &&
        buttonDownTexture_->isWithinBounds (event->motion.x, event->motion.y);
}

void SDLMomentaryButton::handleEvent (SDLEvent const *event)
{
    SDL_Event* sdlEvent = event->getSDL_Event ();

    if (validMouseEvent (sdlEvent))
    {
        switch (sdlEvent->type)
        {
            case SDL_MOUSEBUTTONDOWN:
                buttonState_ = toggleState (naturalState_);
                break;

            case SDL_MOUSEBUTTONUP:
                buttonState_ = naturalState_;
                break;
        }

        buttonClicked_ (buttonState_);
    }
}

