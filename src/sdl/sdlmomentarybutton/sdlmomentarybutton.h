#ifndef _SDLMOMENTARYBUTTON_H_
#define _SDLMOMENTARYBUTTON_H_

#include "panel.h"
#include "../sdlbutton/sdlbutton.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <SDL.h>
#include <string>

using std::string;
using std::unique_ptr;

class SDLMomentaryButton : public SDLButton
{
public:
    SDLMomentaryButton (string buttonDownImage, string buttonUpImage,
        Button::State initialState, unique_ptr<SDLRenderer> &sdlRenderer, 
        EventCallback buttonClicked,
        SDL_Texture* targetTexture, int x, int y, int width, int height);
    ~SDLMomentaryButton ();

    void handleEvent (SDL_Event const *event) override;

private:
    // The natural state of a momentary button is the state the button will
    // revert to onze it is not actuated. That state is either Up or Down as
    // indicated by the constructor's initialState parameter.
    State naturalState_;

    bool validMouseEvent (SDL_Event const *event);
};

#endif // _SDLMOMENTARYBUTTON_H_