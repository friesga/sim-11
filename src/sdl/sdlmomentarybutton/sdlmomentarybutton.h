#ifndef _SDLMOMENTARYBUTTON_H_
#define _SDLMOMENTARYBUTTON_H_

#include "panel.h"
#include "../sdlcommonbutton/sdlcommonbutton.h"
#include "../sdlrenderer/sdlrenderer.h"
#include "../sdlevent/sdlevent.h"

#include <SDL.h>
#include <string>

using std::string;
using std::unique_ptr;

class SDLMomentaryButton : public SDLCommonButton
{
public:
    SDLMomentaryButton (unique_ptr<SDLTexture> buttonDownTexture,
        unique_ptr<SDLTexture> buttonUpTexture,
        Button::TwoPositionsState initialState, EventCallback buttonClicked);
    ~SDLMomentaryButton ();

    void handleEvent (InputEvent const *event) override;

private:
    // The natural state of a momentary button is the state the button will
    // revert to once it is not actuated. That state is either Off or On as
    // indicated by the constructor's initialState parameter.
    TwoPositionsState naturalState_;

    bool validMouseEvent (InputEvent const *event);
};

#endif // _SDLMOMENTARYBUTTON_H_