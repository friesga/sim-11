#ifndef _SDLLATCHINGBUTTON_H_
#define _SDLLATCHINGBUTTON_H_

#include "panel.h"
#include "../sdlbutton/sdlbutton.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <SDL.h>
#include <string>

using std::string;
using std::unique_ptr;

class SDLLatchingButton : public SDLButton
{
public:
    SDLLatchingButton (string buttonDownImage, string buttonUpImage, 
        Button::State initialState, unique_ptr<SDLRenderer> &sdlRenderer,
        EventCallback buttonClicked, int x, int y, int width, int height);
    ~SDLLatchingButton ();

    void handleEvent (SDL_Event const *event) override;
};

#endif // _SDLLATCHINGBUTTON_H_