#ifndef _FAKEINDICATOR_H_
#define _FAKEINDICATOR_H_

#include "panel.h"

class FakeIndicator : public Indicator
{
public:
    void show (State showFigure) override;
    void render () override;
    bool isWithinBounds (Position position, float margin) const override;
};

#endif // _FAKEINDICATOR_H_