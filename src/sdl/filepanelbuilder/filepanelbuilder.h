#ifndef _FILEPANELBUILDER_H_
#define _FILEPANELBUILDER_H_

#include "panel.h"
#include "sdl/include/panelbuilder.h"
#include "sdl/sdltile/sdltile.h"
#include "sdl/sdlrenderer/sdlrenderer.h"
#include "sdl/sdlindicator/sdlindicator.h"
#include "sdl/sdllatchingbutton/sdllatchingbutton.h"
#include "sdl/sdlmomentarybutton/sdlmomentarybutton.h"
#include "sdl/sdlindicatorlatchingbutton/sdlindicatorlatchingbutton.h"
#include "sdl/sdlfourpositionswitch/sdlfourpositionswitch.h"
#include "sdl/sdlthreepositionswitch/sdlthreepositionswitch.h"

#include <string>
#include <utility>

using std::string;
using std::pair;

// This class implements function to create a PanelComposition from
// images in files.
//
class FilePanelBuilder : public PanelBuilder
{
public:
    FilePanelBuilder (unique_ptr<SDLRenderer>& sdlRenderer, SDL_Texture* texture,
        Cabinet::Position cabinetPosition, RackUnit unitHeight);

    // Definition of functions required for the implementation of a Panel
    void createFront (string imageFile,
        Frame<float> frame);
    Indicator* createIndicator (string indicatorOffImage,
        string indicatorOnImage, Indicator::State showFigure,
        Frame<float> frame);
    Button* createLatchingButton (string buttonDownImage, string buttonUpImage,
        Button::TwoPositionsState initialState, Button::EventCallback buttonClicked,
        Frame<float> frame);
    Button* createMomentaryButton (string buttonDownImage, string buttonUpImage,
        Button::TwoPositionsState initialState, Button::EventCallback buttonClicked,
        Frame<float> frame);
    IndicatorButton* createSDLIndicatorLatchingButton (Button::ImageNames const& imageNames,
        Button::TwoPositionsState initialState,
        Button::EventCallback buttonClicked, Indicator::State showIndicator,
        Frame<float> frame);
    Button* createFourPositionSwitch (array<string, 4> positionImages,
        Button::FourPositionsState initialState,
        Button::EventCallback switchClicked,
        Frame<float> frame);
    Button* createThreePositionSwitch (array<string, 3> positionImages,
        Button::ThreePositionsState initialState,
        Button::EventCallback switchClicked,
        Frame<float> frame);

    // Function required by the PanelBuilder interface
    PanelComposition getPanelComposition () override;

private:
    // Storage for all fronts, indicators and buttons part of the
    // PanelComposition.
    vector<unique_ptr<Front>> fronts_;
    vector<unique_ptr<Indicator>> indicators_;
    vector<unique_ptr<Button>> buttons_;
    vector<unique_ptr<IndicatorButton>> indicatorButtons_;

    // Reference to the renderer to use for fronts, indicators and buttons
    unique_ptr<SDLRenderer>& sdlRenderer_;

    // Reference to the panel to draw the panel on
    SDL_Texture* targetTexture_;

    Cabinet::Position cabinetPosition_;

    // The height of the unit in pixels
    float panelHeight_ {0.0f};

    int pixelsPerRackUnit_ {0};

    Frame<int> placeFrameInTexture (Frame<float> frame);
    pair<int, int> getTextureDimensions (SDL_Texture* texture);
};

#endif // _FILEPANELBUILDER_H_