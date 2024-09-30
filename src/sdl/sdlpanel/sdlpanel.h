#ifndef _SDLPANEL_H_
#define _SDLPANEL_H_

#include "panel.h"
#include "../sdlfront/sdlfront.h"
#include "../sdlindicator/sdlindicator.h"
#include "../sdllatchingbutton/sdllatchingbutton.h"
#include "../sdlmomentarybutton/sdlmomentarybutton.h"
#include "../sdlindicatorlatchingbutton/sdlindicatorlatchingbutton.h"
#include "../sdlrenderer/sdlrenderer.h"
#include "../sdlevent/sdlevent.h"
#include "cabinet/cabinet.h"
#include "rackunit.h"

#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class SDLPanel : public Panel
{
public:
    SDLPanel (unique_ptr<SDLRenderer> &sdlRenderer, SDL_Texture* texture,
        shared_ptr<Cabinet::Position> cabinetPosition, RackUnit unitHeight);
    ~SDLPanel ();

    // Definition of functions required for the implementation of a Panel
    virtual void createFront (string imageFile, 
        Frame<float> frame) override;
    virtual Indicator *createIndicator (string imageFile, 
        Indicator::State showFigure, Frame<float> frame) override;
    virtual Button *createLatchingButton (string buttonDownImage, string buttonUpImage,
        Button::State initialState, Button::EventCallback buttonClicked,
        Frame<float> frame) override;
    virtual Button *createMomentaryButton (string buttonDownImage, string buttonUpImage,
        Button::State initialState, Button::EventCallback buttonClicked,
        Frame<float> frame) override;
    virtual Button* createSDLIndicatorLatchingButton (Button::ImageNames const& imageNames,
        Button::State initialState, 
        Button::EventCallback buttonClicked, Indicator::State showIndicator,
        Frame<float> frame) override;

    // SDL implementation specific functions
    virtual void render ();
    virtual void handleEvent (InputEvent const *event);
    bool isOverButton (Position position);

private:
    Frame<int> placeFrameInTexture (Frame<float> frame);
    pair<int, int> getTextureDimensions (SDL_Texture* texture);

    // Reference to the renderer to use for fronts, indicators and buttons
    unique_ptr<SDLRenderer> &sdlRenderer_;

    // Storage for all fronts, indicators and buttons to be rendered
    // on this window
    vector<unique_ptr<Front>> fronts_;
    vector<unique_ptr<Indicator>> indicators_;
    vector<unique_ptr<Button>> buttons_;

    // Reference to the panel to draw the panel on
    SDL_Texture* targetTexture_;

    // Position of the panel in the target texture
    Position panelPosition_ {0, 0};

    // The height of the unit in pixels
    float panelHeight_ {0.0f};
    int pixelsPerRackUnit_ {0};
    shared_ptr<Cabinet::Position> cabinetPosition_;
};

#endif // _SDLPANEL_H_