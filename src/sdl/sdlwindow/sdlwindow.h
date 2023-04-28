#ifndef _SDLWINDOW_H_
#define _SDLWINDOW_H_

#include "panel.h"
#include "../sdlfront/sdlfront.h"
#include "../sdlindicator/sdlindicator.h"
#include "../sdllatchingbutton/sdllatchingbutton.h"
#include "../sdlmomentarybutton/sdlmomentarybutton.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <SDL.h>
#include <vector>

using std::vector;

class SDLWindow : public Window
{
public:
    SDLWindow (char const *title, int x, int y, int width, int height);
    ~SDLWindow ();
    void createFront (string imageFile, 
        int x, int y, int width = 0, int height = 0);
    Indicator *createIndicator (string imageFile, Indicator::State showFigure,
        int x, int y, int width, int height) override;
    Button *createLatchingButton (string buttonDownImage, string buttonUpImage, 
        Button::State initialState, Button::EventCallback buttonClicked, 
        int x, int y, int width = 0, int height = 0) override;
    Button *createMomentaryButton (string buttonDownImage, string buttonUpImage, 
        Button::State initialState, Button::EventCallback buttonClicked,
        int x, int y, int width = 0, int height = 0) override;
    void render () override;
    bool handleEvents ();

private:
    SDL_Window* sdlWindow_;
     
    // The Renderer to use in rendering in the given window
    unique_ptr<SDLRenderer> sdlRenderer_;

    // Storage for all fronts, indicators and buttons to be rendered
    // on this window
    vector<unique_ptr<SDLFront>> fronts_;
    vector<unique_ptr<SDLIndicator>> indicators_;
    vector<unique_ptr<SDLButton>> buttons_;
};


#endif // _SDLWINDOW_H_