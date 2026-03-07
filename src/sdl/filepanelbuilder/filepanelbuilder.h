#ifndef _FILEPANELBUILDER_H_
#define _FILEPANELBUILDER_H_

#include "panel.h"
#include "sdl/sdlfront/sdlfront.h"
#include "sdl/sdlindicator/sdlindicator.h"
#include "sdl/sdlindicatorlatchingbutton/sdlindicatorlatchingbutton.h"
#include "sdl/sdlrenderer/sdlrenderer.h"
#include "sdl/sdlevent/sdlevent.h"

#include <memory>
#include <vector>
#include <array>

using std::unique_ptr;
using std::vector;
using std::array;

class FilePanelBuilder : public PanelBuilder
{
public:
    FilePanelBuilder (unique_ptr<SDLRenderer>& sdlRenderer,
        SDLTexture& texture, Cabinet::Position cabinetPosition, RackUnit unitHeight);

    // Definition of functions required by the PanelBuilder interface
    void createFront (string imageFile,
        Frame<float> frame) override;
    Indicator* createIndicator (string indicatorOffImage,
        string indicatorOnImage, Indicator::State showFigure,
        Frame<float> frame) override;
    Button* createMultiPositionSwitch (vector<string> positionImages,
        Button::State initialState,
        Button::EventCallback switchClicked,
        Frame<float> frame) override;
    IndicatorButton* createIndicatorLatchingButton (Button::ImageNames const& imageNames,
        Button::TwoPositionsState initialState,
        Button::EventCallback buttonClicked, Indicator::State showIndicator,
        Frame<float> frame) override;
    unique_ptr<Panel> getPanel () override;

private:
    int textureHeight (SDLTexture& texture) const;
    int pixelsPerRackUnit () const;
    Frame<int> placeFrameInTexture (Frame<float> frame);

    // Reference to the renderer to use for fronts, indicators and buttons
    unique_ptr<SDLRenderer>& sdlRenderer_;

    // Storage for all fronts, indicators and buttons to be rendered
    // on this window
    vector<unique_ptr<Front>> fronts_;
    vector<unique_ptr<Indicator>> indicators_;
    vector<unique_ptr<Button>> buttons_;
    vector<unique_ptr<IndicatorButton>> indicatorButtons_;

    // Reference to the panel to draw the panel on
    SDLTexture& targetTexture_;

    // Position of the panel in the target texture. The X position
    // currently is unused as just one cabinet is supported.
    Position panelPosition_ {0, 0};

    // The height of the unit in pixels
    float panelHeight_ {0.0f};

    Cabinet::Position cabinetPosition_;
    RackUnit unitHeight_;
};

#endif // _FILEPANELBUILDER_H_
