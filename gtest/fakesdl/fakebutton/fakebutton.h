#ifndef _FAKEBUTTON_H_
#define _FAKEBUTTON_H_

#include "panel.h"

class FakeButton : public Button
{
public:
    void setState (State newState) override;
    State currentState () const override;
    void render () override;
    void handleEvent (InputEvent const* event) override;
    bool isWithinBounds (Position position, float margin) const override;
    void setSwitchClickedCallback (EventCallback callback) override;
};

#endif // _FAKEBUTTON_H_