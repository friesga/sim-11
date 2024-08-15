#include "sdlmomentarybutton.h"


using std::make_unique;

SDLMomentaryButton::SDLMomentaryButton (string buttonDownImage, string buttonUpImage,
    Button::State initialState, unique_ptr<SDLRenderer> &sdlRenderer,
    EventCallback buttonClicked, 
    SDL_Texture* targetTexture, Panel::Frame<float> boundingBox)
    :
    SDLButton (buttonDownImage, buttonUpImage, initialState, sdlRenderer,
        buttonClicked, targetTexture, boundingBox),
    naturalState_ {initialState}
{}

SDLMomentaryButton::~SDLMomentaryButton ()
{}

bool SDLMomentaryButton::validMouseEvent (SDLEvent const *event)
{
    SDL_Event* sdlEvent = event->getSDL_Event ();

    return (sdlEvent->type == SDL_MOUSEBUTTONDOWN || 
        sdlEvent->type == SDL_MOUSEBUTTONUP) &&
        sdlEvent->button.button == SDL_BUTTON_LEFT &&
        isWithinBounds (event->mouseTexturePosition ());
}

void SDLMomentaryButton::handleEvent (SDLEvent const *event)
{
    SDL_Event* sdlEvent = event->getSDL_Event ();

    if (validMouseEvent (event))
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

