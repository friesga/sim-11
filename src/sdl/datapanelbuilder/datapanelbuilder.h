#ifndef _DATAPANELBUILDER_H_
#define _DATAPANELBUILDER_H_

#include "panel.h"
#include "sdl/sdlfront/sdlfront.h"
#include "sdl/sdlindicator/sdlindicator.h"
#include "sdl/sdlindicatorlatchingbutton/sdlindicatorlatchingbutton.h"
#include "sdl/sdlrenderer/sdlrenderer.h"
#include "sdl/sdlevent/sdlevent.h"
#include "imagedata/include/imagecontainer.h"

#include <memory>
#include <vector>
#include <array>

using std::unique_ptr;
using std::vector;
using std::array;

// The DataPanelBuilder provides the functions to create a panel from
// image data stored in an ImageContainer (such as an OpenRaster file).
//
class DataPanelBuilder : public PanelBuilder
{
public:
    DataPanelBuilder (ImageContainer& imageContainer,
        unique_ptr<SDLRenderer>& sdlRenderer, SDLTexture& texture,
        Cabinet::Position cabinetPosition, RackUnit unitHeight);

    // Definition of functions required by the PanelBuilder interface
    virtual void createFront (string imageFile,
        Frame<float> frame = Frame<float> (0, 0, 0, 0)) override;
    virtual Indicator* createIndicator (string indicatorOffImage,
        string indicatorOnImage, Indicator::State showFigure,
        Frame<float> frame) override;
    Button* createNPositionSwitch (vector<string> positionImages,
        Button::State initialState,
        Button::EventCallback switchClicked,
        Frame<float> frame = Frame<float> (0, 0, 0, 0)) override;
    virtual IndicatorButton* createIndicatorLatchingButton (Button::ImageNames const& imageNames,
        Button::TwoPositionsState initialState,
        Button::EventCallback buttonClicked, Indicator::State showIndicator,
        Frame<float> frame) override;
    virtual unique_ptr<Panel> getPanel () override;

private:
    int textureHeight (SDLTexture& texture) const;
    int pixelsPerRackUnit () const;
    Frame<int> placeFrameInTexture (Frame<float> frame);
    Frame<float> getFrameFromImage (string layerName);

    // Reference to the image container to use for loading images
    ImageContainer& imageContainer_;

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

#endif // _SDLFILEPANELBUILDER_H_