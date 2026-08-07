#include "fakebutton.h"

void FakeButton::setState (State newState)
{ }

Button::State FakeButton::currentState () const
{
    return Button::TwoPositionsState::Up;
}

void FakeButton::render ()
{
}

void FakeButton::handleEvent (InputEvent const* event)
{
}

bool FakeButton::isWithinBounds (Position position, float margin) const
{
    return false;
}

void FakeButton::setSwitchClickedCallback (EventCallback callback)
{ }