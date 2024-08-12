#ifndef _SDLPANEL_H_
#define _SDLPANEL_H_

#include "panel.h"
#include "../sdlfront/sdlfront.h"
#include "../sdlindicator/sdlindicator.h"
#include "../sdllatchingbutton/sdllatchingbutton.h"
#include "../sdlmomentarybutton/sdlmomentarybutton.h"
#include "../sdlrenderer/sdlrenderer.h"
#include "../sdlevent/sdlevent.h"

#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class SDLPanel : public Panel
{
public:
    SDLPanel (unique_ptr<SDLRenderer> &sdlRenderer, SDL_Texture* texture);
    ~SDLPanel ();

    // Definition of functions required for the implementation of a Panel
    virtual void createFront (string imageFile, 
        BoundingBox boundingBox) override;
    virtual Indicator *createIndicator (string imageFile, 
        Indicator::State showFigure, BoundingBox boundingBox) override;
    virtual Button *createLatchingButton (string buttonDownImage, string buttonUpImage,
        Button::State initialState, Button::EventCallback buttonClicked,
        BoundingBox boundingBox) override;
    virtual Button *createMomentaryButton (string buttonDownImage, string buttonUpImage,
        Button::State initialState, Button::EventCallback buttonClicked,
        BoundingBox boundingBox) override;

    // SDL implementation specific functions
    virtual void render ();
    virtual void handleEvent (SDLEvent const *event);
    bool isOverButton (Position position);

private:
    // Reference to the renderer to use for fronts, indicators and buttons
    unique_ptr<SDLRenderer> &sdlRenderer_;

    // Storage for all fronts, indicators and buttons to be rendered
    // on this window
    vector<unique_ptr<SDLFront>> fronts_;
    vector<unique_ptr<SDLIndicator>> indicators_;
    vector<unique_ptr<SDLButton>> buttons_;

    // Reference to the panel to draw the panel on
    SDL_Texture* targetTexture_;
};

#endif // _SDLPANEL_H_