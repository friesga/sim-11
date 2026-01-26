#ifndef _MOCKDATAPANELBUILDER_H_
#define _MOCKDATAPANELBUILDER_H_

#include "panel.h"

class FakeDataPanelBuilder : public PanelBuilder
{
public:
    // Functions required by the PanelBuilder interface
    void createFront (string imageFile,
        Frame<float> frame = Frame<float> (0, 0, 0, 0)) override
    {
    }

    Indicator* createIndicator (string indicatorOffImage,
        string indicatorOnImage, Indicator::State showFigure,
        Frame<float> frame = Frame<float> (0, 0, 0, 0)) override
    {
        return nullptr;
    }

    IndicatorButton* createIndicatorLatchingButton (Button::ImageNames const& imageNames,
        Button::TwoPositionsState initialState,
        Button::EventCallback buttonClicked, Indicator::State showIndicator,
        Frame<float> frame) override
    {
        return nullptr;
    }

    Button* createMultiPositionSwitch (vector<string> positionImages,
        Button::State initialState,
        Button::EventCallback switchClicked,
        Frame<float> frame = Frame<float> (0, 0, 0, 0)) override
    {
        return nullptr;
    }

    unique_ptr<Panel> getPanel () override
    {
        return nullptr;
    }
};

#endif // _MOCKDATAPANELBUILDER_H_
