#include "sdlthreepositionswitch.h"

using std::get;

SDLThreePositionSwitch::SDLThreePositionSwitch (PositionTextures positionTextures,
    Button::ThreePositionsState initialState,
    EventCallback switchClicked)
    :
    positionTextures_ {move (positionTextures)},
    switchClicked_ {switchClicked},
    switchPosition_ {initialState}
{}

// Functions required for the Button interface
void SDLThreePositionSwitch::setState (State newState)
{
    switchPosition_ = get<Button::ThreePositionsState> (newState);
}

Button::State SDLThreePositionSwitch::currentState () const
{
    return switchPosition_;
}

void SDLThreePositionSwitch::render ()
{
    positionTextures_[+switchPosition_]->render ();
}

void SDLThreePositionSwitch::handleEvent (InputEvent const* event)
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

bool SDLThreePositionSwitch::isWithinBounds (Position position, float margin) const
{
    return positionTextures_[+switchPosition_]->isWithinBounds (position, margin);
}

bool SDLThreePositionSwitch::isRightOfCenter (Position position, float margin) const
{
    return positionTextures_[+switchPosition_]->isRightOfCenter (position, margin);
}

bool SDLThreePositionSwitch::isLeftOfCenter (Position position, float margin) const
{
    return positionTextures_[+switchPosition_]->isLeftOfCenter (position, margin);
}

Button::ThreePositionsState SDLThreePositionSwitch::nextPosition (Button::ThreePositionsState position)
{
    if (switchPosition_ != Button::ThreePositionsState::Right)
        return static_cast<Button::ThreePositionsState> ((+position) + 1);

    return Button::ThreePositionsState::Right;
}

Button::ThreePositionsState SDLThreePositionSwitch::previousPosition (Button::ThreePositionsState position)
{
    if (switchPosition_ != Button::ThreePositionsState::Left)
        return static_cast<Button::ThreePositionsState> ((+position) - 1);

    return Button::ThreePositionsState::Left;
}
