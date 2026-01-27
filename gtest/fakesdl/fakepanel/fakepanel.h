#ifndef _FAKEPANEL_H_
#define _FAKEPANEL_H_

#include "panel.h"

#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

class FakePanel : public Panel
{
public:
    FakePanel (vector<unique_ptr<Indicator>> indicators,
        vector<unique_ptr<Button>> buttons);
    void render () override;
    void handleEvent (InputEvent const* event) override;
    bool isOverButton (Position position) override;

private:
    vector<unique_ptr<Indicator>> indicators_;
    vector<unique_ptr<Button>> buttons_;
};

#endif _FAKEPANEL_H_