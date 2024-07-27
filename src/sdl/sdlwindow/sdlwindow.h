#ifndef _SDLWINDOW_H_
#define _SDLWINDOW_H_

#include "panel.h"
#include "../sdlpanel/sdlpanel.h"
#include "../sdlrenderer/sdlrenderer.h"
#include "../sdlinit/sdlinit.h"

#include <vector>

using std::vector;

// Create a Window by means of SDL.
//
// The SDL initialization is put in a separate class which we derive from so
// the windows construction code is separated from the SDL initialization
// statements.
class SDLWindow : public Window, public SDLInit
{
public:
    SDLWindow (char const *title, int x, int y, int width, int height);
    ~SDLWindow ();
    Panel *createPanel () override;
    void handler ();


private:
    SDL_Window* sdlWindow_;

    // The Renderer to use in rendering in the given window
    unique_ptr<SDLRenderer> sdlRenderer_; 

    vector<unique_ptr<SDLPanel>> panels_;

    // The target texture the panels have to render to
    SDL_Texture* targetTexture_;

    void render () override;
    bool handleEvents () override;
};

#endif // _SDLWINDOW_H_