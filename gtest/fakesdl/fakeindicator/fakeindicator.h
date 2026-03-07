#ifndef _FAKEINDICATOR_H_
#define _FAKEINDICATOR_H_

#include "panel.h"

class FakeIndicator : public Indicator
{
public:
    Indicator::State indicatorState () const override;
    void show (State showFigure) override;
    void render () override;
    bool isWithinBounds (Position position, float margin) const override;

private:
    Indicator::State indicatorState_ {Indicator::State::Off};
};

#endif // _FAKEINDICATOR_H_