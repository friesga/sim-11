#ifndef _SDLBUTTON_H_
#define _SDLBUTTON_H_

#include "panel.h"
#include "../sdltexture/sdltexture.h"

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
        Button::State initialState, SDL_Renderer *renderer,
        EventCallback buttonClicked, int x, int y, int width, int height);
    ~SDLButton ();

    virtual void handleEvent (SDL_Event const *event) = 0;
    void render ();

protected:
    unique_ptr<SDLTexture> buttonDownTexture_;
    unique_ptr<SDLTexture> buttonUpTexture_;
    EventCallback buttonClicked_;
    State buttonState_;

    State toggleState (State oldState);
};

#endif // _SDLBUTTON_H_