#include "sdlnpositionswitch.h"

using std::get;

template <typename TPositions>
SDLNPositionSwitch<TPositions>::SDLNPositionSwitch (PositionTiles positionTiles,
    TPositions initialState,
    EventCallback switchClicked)
    :
    positionTiles_ {move (positionTiles)},
    switchClicked_ {switchClicked},
    switchPosition_ {initialState}
{
}

// Functions required for the Button interface
template <typename TPositions>
void SDLNPositionSwitch<TPositions>::setState (State newState)
{
    switchPosition_ = get<TPositions> (newState);
}

template <typename TPositions>
Button::State SDLNPositionSwitch<TPositions>::currentState () const
{
    return switchPosition_;
}

template <typename TPositions>
void SDLNPositionSwitch<TPositions>::render ()
{
    positionTiles_[+switchPosition_]->render ();
}

template <typename TPositions>
void SDLNPositionSwitch<TPositions>::handleEvent (InputEvent const* event)
{
    if (event->type () == InputEvent::Type::MouseButtonDown &&
        event->button () == InputEvent::Button::Left &&
        isWithinBounds (event->mousePosition (), centerMargin_))
    {
        if (EnumValue<TPositions>::orientation == Orientation::Centered)
        {
            switchPosition_ = togglePosition (switchPosition_);
            switchClicked_ (switchPosition_);
            return;
        }

        // The orientation is either horizontal or vertical.
        if (switchedUp (event))
        {
            switchPosition_ = nextPosition (switchPosition_);
            switchClicked_ (switchPosition_);
            return;
        }

        if (switchedDown (event))
        {
            switchPosition_ = previousPosition (switchPosition_);
            switchClicked_ (switchPosition_);
            return;
        }
    }

    if (!EnumValue<TPositions>::isLatching &&
        event->type () == InputEvent::Type::MouseButtonUp &&
        event->button () == InputEvent::Button::Left &&
        switchPosition_ == EnumValue<TPositions>::last)
    {
        switchPosition_ = previousPosition (switchPosition_);
        switchClicked_ (switchPosition_);
    }
}

template <typename TPositions>
bool SDLNPositionSwitch<TPositions>::isWithinBounds (Position position, float margin) const
{
    return positionTiles_[+switchPosition_]->isWithinBounds (position, margin);
}

template <typename TPositions>
bool SDLNPositionSwitch<TPositions>::isRightOfCenter (Position position, float margin) const
{
    return positionTiles_[+switchPosition_]->isRightOfCenter (position, margin);
}

template <typename TPositions>
bool SDLNPositionSwitch<TPositions>::isLeftOfCenter (Position position, float margin) const
{
    return positionTiles_[+switchPosition_]->isLeftOfCenter (position, margin);
}

template <typename TPositions>
bool SDLNPositionSwitch<TPositions>::isAboveCenter (Position position, float margin) const
{
    return positionTiles_[+switchPosition_]->isAboveCenter (position, margin);
}

template <typename TPositions>
bool SDLNPositionSwitch<TPositions>::isBelowCenter (Position position, float margin) const
{
    return positionTiles_[+switchPosition_]->isBelowCenter (position, margin);
}

template <typename TPositions>
TPositions SDLNPositionSwitch<TPositions>::nextPosition (TPositions position)
{
    if (switchPosition_ != EnumValue<TPositions>::last)
        return static_cast<TPositions> ((+position) + 1);

    return EnumValue<TPositions>::last;
}

template <typename TPositions>
TPositions SDLNPositionSwitch<TPositions>::togglePosition (TPositions position)
{
    return switchPosition_ == EnumValue<TPositions>::first ?
        EnumValue<TPositions>::last : EnumValue<TPositions>::first;
}

template <typename TPositions>
TPositions SDLNPositionSwitch<TPositions>::previousPosition (TPositions position)
{
    if (switchPosition_ != EnumValue<TPositions>::first)
        return static_cast<TPositions> ((+position) - 1);

    return EnumValue<TPositions>::first;
}

template <typename TPositions>
bool SDLNPositionSwitch<TPositions>::switchedUp (InputEvent const* event) const
{
    return (EnumValue<TPositions>::orientation == Orientation::Horizontal &&
        isRightOfCenter (event->mousePosition (), centerMargin_)) ||
        (EnumValue<TPositions>::orientation == Orientation::Vertical &&
            isAboveCenter (event->mousePosition (), centerMargin_));
}

template <typename TPositions>
bool SDLNPositionSwitch<TPositions>::switchedDown (InputEvent const* event) const
{
    return EnumValue<TPositions>::orientation == Orientation::Horizontal &&
        isLeftOfCenter (event->mousePosition (), centerMargin_) ||
        (EnumValue<TPositions>::orientation == Orientation::Vertical &&
            isBelowCenter (event->mousePosition (), centerMargin_));
}

// Explicit template instantiation to be able to define the methods in
// a separate .cpp file.
template class SDLNPositionSwitch<Button::TwoPositionsState>;
template class SDLNPositionSwitch<Button::MomentaryTwoPositionsState>;
template class SDLNPositionSwitch<Button::CenteredTwoPositionsState>;
template class SDLNPositionSwitch<Button::MomentaryThreePositionsState>;
template class SDLNPositionSwitch<Button::FourPositionsState>;
