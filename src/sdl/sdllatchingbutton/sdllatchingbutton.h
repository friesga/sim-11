#ifndef _SDLLATCHINGBUTTON_H_
#define _SDLLATCHINGBUTTON_H_

#include "panel.h"
#include "../sdlbutton/sdlbutton.h"
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
        Button::State initialState, unique_ptr<SDLRenderer> &sdlRenderer,
        EventCallback buttonClicked, 
        SDL_Texture* targetTexture, Frame<int> frame);
    ~SDLLatchingButton ();

    void handleEvent (SDLEvent const *event) override;
};

#endif // _SDLLATCHINGBUTTON_H_