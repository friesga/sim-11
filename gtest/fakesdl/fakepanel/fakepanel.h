#ifndef _FAKEPANEL_H_
#define _FAKEPANEL_H_

#include "panel.h"

class FakePanel : public Panel
{
public:
    void render () override;
    void handleEvent (InputEvent const* event) override;
    bool isOverButton (Position position) override;
};

#endif _FAKEPANEL_H_