#include "sdlindicatorlatchingbutton.h"

using std::get;

SDLIndicatorLatchingButton::SDLIndicatorLatchingButton (Button::ImageNames const& imageNames,
    Button::TwoPositionsState initialState, unique_ptr<SDLRenderer>& sdlRenderer,
    EventCallback buttonClicked, Indicator::State showIndicator,
    SDL_Texture* targetTexture, Frame<int> frame)
    :
    buttonClicked_ {buttonClicked},
    buttonState_ {initialState},
    indicatorState_ {showIndicator}
{
    textures_[to_integral (Button::TwoPositionsState::Up)][to_integral (Indicator::State::Off)] =
        make_unique<SDLTexture> (imageNames.buttonUpIndicatorOff,
            sdlRenderer->getSDL_Renderer (), targetTexture, frame);
    textures_[to_integral (Button::TwoPositionsState::Down)][to_integral (Indicator::State::Off)] =
        make_unique<SDLTexture> (imageNames.buttonDownIndicatorOff,
            sdlRenderer->getSDL_Renderer (), targetTexture, frame);
    textures_[to_integral (Button::TwoPositionsState::Up)][to_integral (Indicator::State::On)] =
        make_unique<SDLTexture> (imageNames.buttonUpIndicatorOn,
            sdlRenderer->getSDL_Renderer (), targetTexture, frame);
    textures_[to_integral (Button::TwoPositionsState::Down)][to_integral (Indicator::State::On)] =
        make_unique<SDLTexture> (imageNames.buttonDownIndicatorOn,
            sdlRenderer->getSDL_Renderer (), targetTexture, frame);
}

SDLIndicatorLatchingButton::~SDLIndicatorLatchingButton ()
{}

SDLTexture* SDLIndicatorLatchingButton::getTexture (Button::TwoPositionsState buttonState,
    Indicator::State indicatorState) const
{
    return textures_[to_integral (buttonState)][to_integral (indicatorState)].get ();
}

void SDLIndicatorLatchingButton::setState (Button::State newState)
{
    buttonState_ = get<Button::TwoPositionsState> (newState);
}

// Definition of functions required for the Button interface
void SDLIndicatorLatchingButton::handleEvent (InputEvent const* event)
{
    if (event->type () == InputEvent::Type::MouseButtonDown &&
        event->button () == InputEvent::Button::Left &&
        isWithinBounds (event->mousePosition ()))
    {
        buttonState_ = (buttonState_ == Button::TwoPositionsState::Up) ? 
            Button::TwoPositionsState::Down : Button::TwoPositionsState::Up; 
        buttonClicked_ (buttonState_);
    }
}

void SDLIndicatorLatchingButton::render ()
{
    getTexture (buttonState_, indicatorState_)->render ();
}

bool SDLIndicatorLatchingButton::isWithinBounds (Position position,
    float margin) const
{
    // Any of the textures can be used to check the dimensions
    return getTexture (Button::TwoPositionsState::Up,
        Indicator::State::Off)->isWithinBounds (position, margin);
}

// Definition of functions required for the Indicator interface
void SDLIndicatorLatchingButton::show (Indicator::State indicatorState)
{
    indicatorState_ = indicatorState;
}