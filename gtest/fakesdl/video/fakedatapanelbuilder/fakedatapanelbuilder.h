#ifndef _FAKEDATAPANELBUILDER_H_
#define _FAKEDATAPANELBUILDER_H_

#include "panel.h"

#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

class FakeDataPanelBuilder : public PanelBuilder
{
public:
    // Functions required by the PanelBuilder interface
    void createFront (string imageFile,
        Frame<float> frame = Frame<float> (0, 0, 0, 0)) override;
    Indicator* createIndicator (string indicatorOffImage,
        string indicatorOnImage, Indicator::State showFigure,
        Frame<float> frame = Frame<float> (0, 0, 0, 0)) override;
    IndicatorButton* createIndicatorLatchingButton (Button::ImageNames const& imageNames,
        Button::TwoPositionsState initialState,
        Button::EventCallback buttonClicked, Indicator::State showIndicator,
        Frame<float> frame) override;
    Button* createMultiPositionSwitch (vector<string> positionImages,
        Button::State initialState,
        Button::EventCallback switchClicked,
        Frame<float> frame = Frame<float> (0, 0, 0, 0)) override;
    unique_ptr<Panel> getPanel () override;

private:
    vector<unique_ptr<Indicator>> indicators_;
    vector<unique_ptr<Button>> buttons_;
};

#endif // _FAKEDATAPANELBUILDER_H_
