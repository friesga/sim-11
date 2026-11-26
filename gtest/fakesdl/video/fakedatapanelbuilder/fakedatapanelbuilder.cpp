#include "fakedatapanelbuilder.h"
#include "../fakeindicator/fakeindicator.h"
#include "../fakebutton/fakebutton.h"
#include "../fakepanel/fakepanel.h"
#include "../fakefront/fakefront.h"

using std::make_unique;

Front* FakeDataPanelBuilder::createFront (string imageFile,
    Front::EventHandler eventHandler, Frame<float> frame)
{
    fronts_.push_back (make_unique<FakeFront> ());
    return fronts_.back ().get ();
}

Indicator* FakeDataPanelBuilder::createIndicator (string indicatorOffImage,
    string indicatorOnImage, Indicator::State showFigure,
    Frame<float> frame)
{
    indicators_.push_back (make_unique<FakeIndicator> ());
    return indicators_.back ().get ();
}

IndicatorButton* FakeDataPanelBuilder::createIndicatorLatchingButton (Button::ImageNames const& imageNames,
    Button::TwoPositionsState initialState,
    Button::EventCallback buttonClicked, Indicator::State showIndicator,
    Frame<float> frame)
{
    return nullptr;
}

Button* FakeDataPanelBuilder::createMultiPositionSwitch (vector<string> positionImages,
    Button::State initialState,
    Button::EventCallback switchClicked,
    Frame<float> frame)
{
    buttons_.push_back (make_unique<FakeButton> ());
    return buttons_.back ().get ();
}

unique_ptr<Panel> FakeDataPanelBuilder::getPanel ()
{
    return make_unique<FakePanel> (move (indicators_), move (buttons_));
}