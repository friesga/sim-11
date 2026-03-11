#include "sdl/sdlwindow/sdlwindow.h"
#include "cabinet/cabinet.h"

#include <SDL.h>
#include <SDL_image.h>
#include <emscripten.h>
#include <iostream>

static const double h9642AspectRatio = 19.0 / 35.0;
static const int windowWidth = 500;
const RackUnit unitHeight {3_ru};
Frame<float> ba11_nFrontFrame {0, 0, 1.0, 1.0};

SDLWindow* sdlWindow;

static void renderLoop ()
{
    sdlWindow->renderWrapper ();
}

int main ()
{
    SDL_Init (SDL_INIT_VIDEO);
    IMG_Init (IMG_INIT_PNG);

    sdlWindow  = new SDLWindow {"SDL Window test", {100, 100, windowWidth,
        static_cast<int> (windowWidth / h9642AspectRatio)},
        {Window::Flag::WindowHidden}};
    /*
    sdlWindow = new SDLWindow {"SDL Window test", {SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600},
        {Window::Flag::WindowShown}};
    */

    unique_ptr<PanelBuilder> panelBuilder =
        sdlWindow->createFilePanelBuilder (Cabinet::Position {0, 10_ru}, unitHeight);

    panelBuilder->createFront ("/resources/11_03 front.png", ba11_nFrontFrame);

    sdlWindow->addPanel (panelBuilder->getPanel ());

    sdlWindow->show ();

    // Argument 3 (set_infinite_loop) is set to true to prevent the browser
    // from timing out the main loop after a certain amount of time. This is
    // necessary as the main loop will run indefinitely until the user closes
    // the page, and we don't want the browser to think that the page has
    // become unresponsive.
    emscripten_set_main_loop (renderLoop, 0, 1);
}