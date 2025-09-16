#ifndef _SDLLATCHINGBUTTON_H_
#define _SDLLATCHINGBUTTON_H_

#include "panel.h"
#include "../sdlcommonbutton/sdlcommonbutton.h"
#include "../sdlrenderer/sdlrenderer.h"
#include "../sdlevent/sdlevent.h"

#include <SDL.h>
#include <string>

using std::string;
using std::unique_ptr;

class SDLLatchingButton : public SDLCommonButton
{
public:
    SDLLatchingButton (string buttonDownImage, string buttonUpImage, 
        Button::TwoPositionsState initialState, unique_ptr<SDLRenderer> &sdlRenderer,
        EventCallback buttonClicked, 
        SDL_Texture* targetTexture, Frame<int> frame);
    SDLLatchingButton (unique_ptr<SDLTexture> buttonDownTexture,
        unique_ptr<SDLTexture> buttonUpTexture,
        Button::TwoPositionsState initialState,
        EventCallback buttonClicked);
    ~SDLLatchingButton ();

    void handleEvent (InputEvent const *event) override;
};

#endif // _SDLLATCHINGBUTTON_H_