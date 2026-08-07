#include "fakepanel.h"

FakePanel::FakePanel (vector<unique_ptr<Indicator>> indicators,
    vector<unique_ptr<Button>> buttons)
    :
    indicators_ {move (indicators)},
    buttons_ {move (buttons)}
{}

void FakePanel::render ()
{}

void FakePanel::handleEvent (InputEvent const* event)
{}

bool FakePanel::isOverButton (Position position)
{
    return false;
}
