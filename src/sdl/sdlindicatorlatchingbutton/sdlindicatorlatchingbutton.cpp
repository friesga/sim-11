#include "sdlindicatorlatchingbutton.h"

using std::get;

SDLIndicatorLatchingButton::SDLIndicatorLatchingButton (TileGrid tiles,
    Button::TwoPositionsState initialState, EventCallback buttonClicked,
    Indicator::State showIndicator)
    :
    tiles_ {move (tiles)},
    buttonClicked_ {buttonClicked},
    buttonState_ {initialState},
    indicatorState_ {showIndicator}
{}

SDLIndicatorLatchingButton::~SDLIndicatorLatchingButton ()
{}

SDLTile* SDLIndicatorLatchingButton::getTile (Button::TwoPositionsState buttonState,
    Indicator::State indicatorState) const
{
    return tiles_[to_integral (buttonState)][to_integral (indicatorState)].get ();
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
        buttonState_ = (buttonState_ == Button::TwoPositionsState::Up) ? 
            Button::TwoPositionsState::Down : Button::TwoPositionsState::Up; 
        buttonClicked_ (buttonState_);
    }
}

void SDLIndicatorLatchingButton::render ()
{
    getTile (buttonState_, indicatorState_)->render ();
}

bool SDLIndicatorLatchingButton::isWithinBounds (Position position,
    float margin) const
{
    // Any of the tiles can be used to check the dimensions
    return getTile (Button::TwoPositionsState::Up,
        Indicator::State::Off)->isWithinBounds (position, margin);
}

// Definition of functions required for the Indicator interface
Indicator::State SDLIndicatorLatchingButton::indicatorState () const
{
    return indicatorState_;
}

void SDLIndicatorLatchingButton::show (Indicator::State indicatorState)
{
    indicatorState_ = indicatorState;
}