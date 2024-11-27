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
    SDLMomentaryButton (string buttonDownImage, string buttonUpImage,
        Button::TwoPositionsState initialState, unique_ptr<SDLRenderer> &sdlRenderer, 
        EventCallback buttonClicked,
        SDL_Texture* targetTexture, Frame<int> frame);
    ~SDLMomentaryButton ();

    void handleEvent (InputEvent const *event) override;

private:
    // The natural state of a momentary button is the state the button will
    // revert to once it is not actuated. That state is either Up or Down as
    // indicated by the constructor's initialState parameter.
    TwoPositionsState naturalState_;

    bool validMouseEvent (InputEvent const *event);
};

#endif // _SDLMOMENTARYBUTTON_H_