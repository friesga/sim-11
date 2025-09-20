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
    SDLLatchingButton (unique_ptr<SDLTile> buttonDownTexture,
        unique_ptr<SDLTile> buttonUpTexture,
        Button::TwoPositionsState initialState,
        EventCallback buttonClicked);
    ~SDLLatchingButton ();

    void handleEvent (InputEvent const *event) override;
};

#endif // _SDLLATCHINGBUTTON_H_