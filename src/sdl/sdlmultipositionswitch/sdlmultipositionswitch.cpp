#include "sdlmultipositionswitch.h"

using std::get;

template <typename TPositions>
SDLMultiPositionSwitch<TPositions>::SDLMultiPositionSwitch (PositionTiles positionTiles,
    TPositions initialState,
    EventCallback switchClicked)
    :
    switchClicked_ {switchClicked},
    switchPosition_ {initialState},
    positionTiles_ {move (positionTiles)}
{
}

// Functions required for the Button interface
template <typename TPositions>
void SDLMultiPositionSwitch<TPositions>::setState (State newState)
{
    switchPosition_ = get<TPositions> (newState);
}

template <typename TPositions>
Button::State SDLMultiPositionSwitch<TPositions>::currentState () const
{
    return switchPosition_;
}

template <typename TPositions>
void SDLMultiPositionSwitch<TPositions>::render ()
{
    positionTiles_[+switchPosition_]->render ();
}

template <typename TPositions>
void SDLMultiPositionSwitch<TPositions>::handleEvent (InputEvent const* event)
{
    if (event->type () == InputEvent::Type::MouseButtonDown &&
        event->button () == InputEvent::Button::Left)
    {
        if (ButtonTrait<TPositions>::orientation == Orientation::Centered &&
            isWithinBounds (event->mousePosition (), centerMargin_))
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

    if (!ButtonTrait<TPositions>::isLatching &&
        event->type () == InputEvent::Type::MouseButtonUp &&
        event->button () == InputEvent::Button::Left &&
        switchPosition_ == ButtonTrait<TPositions>::last)
    {
        switchPosition_ = previousPosition (switchPosition_);
        switchClicked_ (switchPosition_);
    }
}

template <typename TPositions>
bool SDLMultiPositionSwitch<TPositions>::isWithinBounds (Position position, float margin) const
{
    return positionTiles_[+switchPosition_]->isWithinBounds (position, margin);
}

template <typename TPositions>
bool SDLMultiPositionSwitch<TPositions>::isRightOfCenter (Position position, float margin) const
{
    return positionTiles_[+switchPosition_]->isRightOfCenter (position, margin);
}

template <typename TPositions>
bool SDLMultiPositionSwitch<TPositions>::isLeftOfCenter (Position position, float margin) const
{
    return positionTiles_[+switchPosition_]->isLeftOfCenter (position, margin);
}

template <typename TPositions>
bool SDLMultiPositionSwitch<TPositions>::isAboveCenter (Position position, float margin) const
{
    return positionTiles_[+switchPosition_]->isAboveCenter (position, margin);
}

template <typename TPositions>
bool SDLMultiPositionSwitch<TPositions>::isBelowCenter (Position position, float margin) const
{
    return positionTiles_[+switchPosition_]->isBelowCenter (position, margin);
}

template <typename TPositions>
TPositions SDLMultiPositionSwitch<TPositions>::nextPosition (TPositions position)
{
    if (switchPosition_ != ButtonTrait<TPositions>::last)
        return static_cast<TPositions> ((+position) + 1);

    return ButtonTrait<TPositions>::last;
}

template <typename TPositions>
TPositions SDLMultiPositionSwitch<TPositions>::togglePosition (TPositions position)
{
    return switchPosition_ == ButtonTrait<TPositions>::first ?
        ButtonTrait<TPositions>::last : ButtonTrait<TPositions>::first;
}

template <typename TPositions>
TPositions SDLMultiPositionSwitch<TPositions>::previousPosition (TPositions position)
{
    if (switchPosition_ != ButtonTrait<TPositions>::first)
        return static_cast<TPositions> ((+position) - 1);

    return ButtonTrait<TPositions>::first;
}

template <typename TPositions>
bool SDLMultiPositionSwitch<TPositions>::switchedUp (InputEvent const* event) const
{
    return (ButtonTrait<TPositions>::orientation == Orientation::Horizontal &&
        isRightOfCenter (event->mousePosition (), centerMargin_)) ||
        (ButtonTrait<TPositions>::orientation == Orientation::Vertical &&
            isAboveCenter (event->mousePosition (), centerMargin_));
}

template <typename TPositions>
bool SDLMultiPositionSwitch<TPositions>::switchedDown (InputEvent const* event) const
{
    return (ButtonTrait<TPositions>::orientation == Orientation::Horizontal &&
        isLeftOfCenter (event->mousePosition (), centerMargin_)) ||
        (ButtonTrait<TPositions>::orientation == Orientation::Vertical &&
            isBelowCenter (event->mousePosition (), centerMargin_));
}

template <typename TPositions>
void SDLMultiPositionSwitch<TPositions>::setSwitchClickedCallback (EventCallback callback)
{
    switchClicked_ = callback;
}

// Explicit template instantiation to be able to define the methods in
// a separate .cpp file.
template class SDLMultiPositionSwitch<Button::TwoPositionsState>;
template class SDLMultiPositionSwitch<Button::MomentaryUpTwoPositionsState>;
template class SDLMultiPositionSwitch<Button::MomentaryDownTwoPositionsState>;
template class SDLMultiPositionSwitch<Button::CenteredTwoPositionsState>;
template class SDLMultiPositionSwitch<Button::ThreePositionsState>;
template class SDLMultiPositionSwitch<Button::MomentaryThreePositionsState>;
template class SDLMultiPositionSwitch<Button::FourPositionsState>;
