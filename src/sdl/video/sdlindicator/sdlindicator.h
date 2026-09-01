#ifndef _SDLINDICATOR_H_
#define _SDLINDICATOR_H_

#include "panel.h"
#include "../sdltile/sdltile.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <SDL3/SDL.h>

using std::string;
using std::unique_ptr;

class SDLIndicator : public Indicator
{
public:
    SDLIndicator (unique_ptr<SDLTile> indicatorOffTile,
        unique_ptr<SDLTile> indicatorOnTile, State showIndicator);
    ~SDLIndicator ();

    // Definition of functions required for the Indicator interface
    Indicator::State indicatorState () const override;
    void show (Indicator::State showIndicator) override;
    void render () override;

    bool isWithinBounds (Position position, float margin) const override;

private:
    // Indication whether or not the indicator has to be shown
    // Inicidators are set from other threads than the SDL rendering thread,
    // so the showIndicator_ really should be atomic. As long as this doesn't
    // cause any visable problems, we will leave it as is for now.
    //
    Indicator::State showIndicator_;

    // The tiles to use for this indicator
    unique_ptr<SDLTile> indicatorOffTile_;
    unique_ptr<SDLTile> indicatorOnTile_;
};

#endif // _SDLINDICATOR_H_