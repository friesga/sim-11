#ifndef _DUMMYCONTROLS_H_
#define _DUMMYCONTROLS_H_

#include "panel.h"

// This file contains dummy panel elements to be used to avoid
// null pointer references.
class DummyIndicator : public Indicator
{
public:
    Indicator::State indicatorState () const { return Indicator::State::Off; }
    void show (Indicator::State showFigure) {}
    void render () {}
    bool isWithinBounds (Position position, float margin) const { return true; }
};

class DummyButton : public Button
{
public:
    DummyButton (State initialState) : state_ {initialState} {}

    // Functions required by the Button interface
    void setState (State newState) { state_ = newState;  }
    State currentState () const { return state_; }
    void render () {}
    void handleEvent (InputEvent const* event) {}
    bool isWithinBounds (Position position, float margin) const { return true; }
    void setSwitchClickedCallback (EventCallback callback) {}

private:
    State state_;
};

class DummyIndicatorButton : public IndicatorButton
{
public:
    void render () {}
    void setState (Button::State newState) {}
    Button::State currentState () const { return TwoPositionsState::Up; }
    void handleEvent (InputEvent const* event) {}
    bool isWithinBounds (Position position, float margin) const { return true; }
    void setSwitchClickedCallback (EventCallback callback) {}
    void setSwitchClicked (EventCallback callback) {}
    void show (Indicator::State showFigure) {}
    Indicator::State indicatorState () const { return Indicator::State::Off; }
};

#endif // _DUMMYCONTROLS_H_