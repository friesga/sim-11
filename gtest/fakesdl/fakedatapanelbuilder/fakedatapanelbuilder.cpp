#include "fakedatapanelbuilder.h"

void FakeDataPanelBuilder::createFront (string imageFile,
    Frame<float> frame)
{
}

Indicator* FakeDataPanelBuilder::createIndicator (string indicatorOffImage,
    string indicatorOnImage, Indicator::State showFigure,
    Frame<float> frame)
{
    return nullptr;
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
    return nullptr;
}

unique_ptr<Panel> FakeDataPanelBuilder::getPanel ()
{
    return nullptr;
}