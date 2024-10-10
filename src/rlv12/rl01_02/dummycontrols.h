#ifndef _DUMMYCONTROLS_H_
#define _DUMMYCONTROLS_H_

#include "panel.h"

// This file contains dummy panel elements to be used to avoid
// null pointer references.
class DummyIndicator : public Indicator
{
public:
    void show (State showFigure) {};
    void render () {};
};

class DummyIndicatorButton : public IndicatorButton
{
public:
    void render () {};
    void setState (Button::State newState) {};
    void handleEvent (InputEvent const* event) {};
    bool isWithinBounds (Position position, float margin) const { return true; };
    void show (Indicator::State showFigure) {};
};

#endif // _DUMMYCONTROLS_H_