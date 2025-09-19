#include "sdlfourpositionswitch.h"

using std::get;

SDLFourPositionSwitch::SDLFourPositionSwitch (PositionTextures positionTextures,
    Button::FourPositionsState initialState,
    EventCallback switchClicked)
    :
    positionTextures_ {move (positionTextures)},
    switchClicked_ {switchClicked},
    switchPosition_ {initialState}
{}

void SDLFourPositionSwitch::setState (State newState)
{
    switchPosition_ = get<Button::FourPositionsState> (newState);
}

Button::State SDLFourPositionSwitch::currentState () const
{
    return switchPosition_;
}

void SDLFourPositionSwitch::render ()
{
    positionTextures_[+switchPosition_]->render ();
}

void SDLFourPositionSwitch::handleEvent (InputEvent const* event)
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
}

bool SDLFourPositionSwitch::isWithinBounds (Position position, float margin) const
{ 
    return positionTextures_[+switchPosition_]->isWithinBounds (position, margin);
}

bool SDLFourPositionSwitch::isRightOfCenter (Position position, float margin) const
{
    return positionTextures_[+switchPosition_]->isRightOfCenter (position, margin);
}

bool SDLFourPositionSwitch::isLeftOfCenter (Position position, float margin) const
{
    return positionTextures_[+switchPosition_]->isLeftOfCenter (position, margin);
}

Button::FourPositionsState SDLFourPositionSwitch::nextPosition (Button::FourPositionsState position)
{
    if (switchPosition_ != Button::FourPositionsState::P3)
        return static_cast<Button::FourPositionsState> ((+position) + 1);

    return Button::FourPositionsState::P3;
}

Button::FourPositionsState SDLFourPositionSwitch::previousPosition (Button::FourPositionsState position)
{
    if (switchPosition_ != Button::FourPositionsState::P0)
        return static_cast<Button::FourPositionsState> ((+position) - 1);

    return Button::FourPositionsState::P0;
}