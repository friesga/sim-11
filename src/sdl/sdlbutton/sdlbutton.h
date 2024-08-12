#ifndef _SDLBUTTON_H_
#define _SDLBUTTON_H_

#include "panel.h"
#include "../sdltexture/sdltexture.h"
#include "../sdlrenderer/sdlrenderer.h"
#include "../sdlevent/sdlevent.h"

#include <SDL.h>
#include <string>

using std::string;
using std::unique_ptr;

// SDLButton is an abstract base class for the SDLLatchingButton and
// SDLMomentary classes. These last two classes differ in handling of the
// button clicked event.
class SDLButton : public Button
{
public:
    SDLButton (string buttonDownImage, string buttonUpImage, 
        Button::State initialState, unique_ptr<SDLRenderer> &sdlRenderer,
        EventCallback buttonClicked, 
        SDL_Texture* targetTexture, Panel::BoundingBox boundingBox);
    ~SDLButton ();

    virtual void handleEvent (SDLEvent const *event) = 0;
    void render ();
    bool isWithinBounds (Position position) const;

protected:
    unique_ptr<SDLTexture> buttonDownTexture_;
    unique_ptr<SDLTexture> buttonUpTexture_;
    EventCallback buttonClicked_;
    State buttonState_;

    State toggleState (State oldState);    
};

#endif // _SDLBUTTON_H_