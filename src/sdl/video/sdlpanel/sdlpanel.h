#ifndef _SDLPANEL_H_
#define _SDLPANEL_H_

#include "panel.h"
#include "../sdlfront/sdlfront.h"
#include "../sdlindicator/sdlindicator.h"
#include "../sdlindicatorlatchingbutton/sdlindicatorlatchingbutton.h"
#include "../sdlrenderer/sdlrenderer.h"
#include "../sdlevent/sdlevent.h"
#include "cabinet/cabinet.h"
#include "rackunit.h"

#include <memory>
#include <vector>
#include <array>

using std::unique_ptr;
using std::make_unique;
using std::vector;
using std::array;

class SDLPanel : public Panel
{
public:
    SDLPanel (unique_ptr<SDLRenderer> &sdlRenderer, SDLTexture& texture,
        Cabinet::Position cabinetPosition, RackUnit unitHeight);
    SDLPanel (unique_ptr<SDLRenderer>& sdlRenderer,
        SDLTexture& texture, Cabinet::Position cabinetPosition,
        RackUnit unitHeight, PanelComposition panelComposition);
    ~SDLPanel ();

    // Functions required by the Panel interface
    void render () override;
    void handleEvent (InputEvent const *event) override;
    bool isOverButton (Position position) override;

private:
    // Reference to the renderer to use for fronts, indicators and buttons
    unique_ptr<SDLRenderer> &sdlRenderer_;

    // Storage for all fronts, indicators and buttons to be rendered
    // on this window
    vector<unique_ptr<Front>> fronts_;
    vector<unique_ptr<Indicator>> indicators_;
    vector<unique_ptr<Button>> buttons_;
    vector<unique_ptr<IndicatorButton>> indicatorButtons_;

    // Reference to the panel to draw the panel on
    SDLTexture& targetTexture_;

    // Position of the panel in the target texture
    Position panelPosition_ {0, 0};

    // The height of the unit in pixels
    float panelHeight_ {0.0f};
    int pixelsPerRackUnit_ {0};
    Cabinet::Position cabinetPosition_;
};

#endif // _SDLPANEL_H_