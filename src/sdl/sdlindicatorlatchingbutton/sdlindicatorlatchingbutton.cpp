#include "sdlindicatorlatchingbutton.h"

using std::get;

SDLIndicatorLatchingButton::SDLIndicatorLatchingButton (TextureGrid textures,
    Button::TwoPositionsState initialState, EventCallback buttonClicked,
    Indicator::State showIndicator)
    :
    textures_ {move (textures)},
    buttonClicked_ {buttonClicked},
    buttonState_ {initialState},
    indicatorState_ {showIndicator}
{}

SDLIndicatorLatchingButton::~SDLIndicatorLatchingButton ()
{}

SDLTile* SDLIndicatorLatchingButton::getTexture (Button::TwoPositionsState buttonState,
    Indicator::State indicatorState) const
{
    return textures_[to_integral (buttonState)][to_integral (indicatorState)].get ();
}

void SDLIndicatorLatchingButton::setState (Button::State newState)
{
    buttonState_ = get<Button::TwoPositionsState> (newState);
}

Button::State SDLIndicatorLatchingButton::currentState () const
{
    return buttonState_;
}

// Definition of functions required for the Button interface
void SDLIndicatorLatchingButton::handleEvent (InputEvent const* event)
{
    if (event->type () == InputEvent::Type::MouseButtonDown &&
        event->button () == InputEvent::Button::Left &&
        isWithinBounds (event->mousePosition ()))
    {
        buttonState_ = (buttonState_ == Button::TwoPositionsState::Off) ? 
            Button::TwoPositionsState::On : Button::TwoPositionsState::Off; 
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
    return getTexture (Button::TwoPositionsState::Off,
        Indicator::State::Off)->isWithinBounds (position, margin);
}

// Definition of functions required for the Indicator interface
void SDLIndicatorLatchingButton::show (Indicator::State indicatorState)
{
    indicatorState_ = indicatorState;
}