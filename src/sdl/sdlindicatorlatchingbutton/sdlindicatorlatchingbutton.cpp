#include "sdlindicatorlatchingbutton.h"

SDLIndicatorLatchingButton::SDLIndicatorLatchingButton (Params const& params,
    Button::State initialState, unique_ptr<SDLRenderer>& sdlRenderer,
    EventCallback buttonClicked, Indicator::State showIndicator,
    SDL_Texture* targetTexture, Frame<int> frame)
    :
    buttonClicked_ {buttonClicked},
    buttonState_ {Button::State::Up},
    indicatorState_ {Indicator::State::Off}
{
    textures_[to_integral (Button::State::Up)][to_integral (Indicator::State::Off)] =
        make_unique<SDLTexture> (params.buttonUpIndicatorOff,
            sdlRenderer->getSDL_Renderer (), targetTexture, frame);
    textures_[to_integral (Button::State::Down)][to_integral (Indicator::State::Off)] =
        make_unique<SDLTexture> (params.buttonDownIndicatorOff,
            sdlRenderer->getSDL_Renderer (), targetTexture, frame);
    textures_[to_integral (Button::State::Up)][to_integral (Indicator::State::On)] =
        make_unique<SDLTexture> (params.buttonUpIndicatorOn,
            sdlRenderer->getSDL_Renderer (), targetTexture, frame);
    textures_[to_integral (Button::State::Down)][to_integral (Indicator::State::On)] =
        make_unique<SDLTexture> (params.buttonDownIndicatorOn,
            sdlRenderer->getSDL_Renderer (), targetTexture, frame);
}

SDLIndicatorLatchingButton::~SDLIndicatorLatchingButton ()
{}

SDLTexture* SDLIndicatorLatchingButton::getTexture (Button::State buttonState,
    Indicator::State indicatorState) const
{
    return textures_[to_integral (buttonState)][to_integral (indicatorState)].get ();
}

// Definition of functions required for the Button interface
void SDLIndicatorLatchingButton::handleEvent (SDLEvent const* event)
{
    SDL_Event* sdlEvent = event->getSDL_Event ();

    if (sdlEvent->type == SDL_MOUSEBUTTONDOWN &&
        sdlEvent->button.button == SDL_BUTTON_LEFT &&
        isWithinBounds (event->mouseTexturePosition ()))
    {
        buttonState_ = (buttonState_ == Button::State::Up) ? 
            Button::State::Down : Button::State::Up; 
        buttonClicked_ (buttonState_);
    }
}

void SDLIndicatorLatchingButton::render ()
{
    getTexture (buttonState_, indicatorState_)->render ();
}

// Definition of functions required for the Indicator interface
void SDLIndicatorLatchingButton::show (Indicator::State indicatorState)
{
    indicatorState_ = indicatorState;
}