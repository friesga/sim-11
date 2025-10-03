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
        isWithinBounds (event->mousePosition (), 0.75f))
    {
        if (isRightOfCenter (event->mousePosition (), 0.75f))
        {
            switchPosition_ = nextPosition (switchPosition_);
            switchClicked_ (switchPosition_);
            return;
        }

        if (isLeftOfCenter (event->mousePosition (), 0.75f))
        {
            switchPosition_ = previousPosition (switchPosition_);
            switchClicked_ (switchPosition_);
            return;
        }
    }

    if (event->type () == InputEvent::Type::MouseButtonUp &&
        event->button () == InputEvent::Button::Left &&
        switchPosition_ == Button::ThreePositionsState::Right)
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
TPositions SDLNPositionSwitch<TPositions>::nextPosition (TPositions position)
{
    if (switchPosition_ != EnumValue<TPositions>::last)
        return static_cast<TPositions> ((+position) + 1);

    return EnumValue<TPositions>::last;
}

template <typename TPositions>
TPositions SDLNPositionSwitch<TPositions>::previousPosition (TPositions position)
{
    if (switchPosition_ != EnumValue<TPositions>::first)
        return static_cast<TPositions> ((+position) - 1);

    return EnumValue<TPositions>::last;
}

// Explicit template instantiation to be able to define the methods in
// a separate .cpp file.
template class SDLNPositionSwitch<Button::ThreePositionsState>;
