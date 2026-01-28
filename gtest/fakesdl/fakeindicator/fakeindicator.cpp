#include "fakeindicator.h"

Indicator::State FakeIndicator::indicatorState () const
{
    return indicatorState_;
}

void FakeIndicator::show (State showFigure)
{
    indicatorState_ = showFigure;
}

void FakeIndicator::render ()
{ }

bool FakeIndicator::isWithinBounds (Position position, float margin) const
{
    return false;
}