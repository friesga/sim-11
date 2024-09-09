#ifndef _SDLCOMMONBUTTON_H_
#define _SDLCOMMONBUTTON_H_

#include "panel.h"
#include "../sdltexture/sdltexture.h"
#include "../sdlrenderer/sdlrenderer.h"
#include "../sdlevent/sdlevent.h"

#include <SDL.h>
#include <string>

using std::string;
using std::unique_ptr;

// SDLCommonButton is an abstract base class for the SDLLatchingButton and
// SDLMomentary classes. These last two classes differ in handling of the
// button clicked event.
class SDLCommonButton : public Button
{
public:
    SDLCommonButton (string buttonDownImage, string buttonUpImage, 
        Button::State initialState, unique_ptr<SDLRenderer> &sdlRenderer,
        EventCallback buttonClicked, 
        SDL_Texture* targetTexture, Frame<int> frame);
    ~SDLCommonButton ();

    virtual void handleEvent (SDLEvent const *event) = 0;
    void render ();
    bool isWithinBounds (Position position, float margin = 0.0) const;

protected:
    unique_ptr<SDLTexture> buttonDownTexture_;
    unique_ptr<SDLTexture> buttonUpTexture_;
    EventCallback buttonClicked_;
    State buttonState_;

    State toggleState (State oldState);    
};

#endif // _SDLCOMMONBUTTON_H_