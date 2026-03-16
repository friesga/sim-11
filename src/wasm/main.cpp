#if 1
#include "sdl/sdlwindow/sdlwindow.h"
#include "cabinet/cabinet.h"
#include "ba11/ba11_n/ba11_n.h"
#include "bus/qbus/qbus.h"

#include <SDL.h>
#include <SDL_image.h>
#include <emscripten.h>
#include <html5.h>
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

    emscripten_set_canvas_element_size ("#canvas", 500, 1000);
    emscripten_set_element_css_size ("#canvas", 500, 1000);

    Qbus bus {};

    sdlWindow  = new SDLWindow {"SDL Window test", {100, 100, windowWidth,
        static_cast<int> (windowWidth / h9642AspectRatio)},
        {Window::Flag::WindowHidden}};

    BA11_N ba11_n {&bus, sdlWindow, BA11_NConfig {Cabinet::Position {0, 10_ru}}};

    // Argument 3 (set_infinite_loop) is set to true to prevent the browser
    // from timing out the main loop after a certain amount of time. This is
    // necessary as the main loop will run indefinitely until the user closes
    // the page, and we don't want the browser to think that the page has
    // become unresponsive.
    emscripten_set_main_loop (renderLoop, 0, 1);
}
#else
#include <SDL.h>
#include <SDL_image.h>
#include <emscripten.h>
#include <html5.h>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* imageTexture = nullptr;
SDL_Texture* windowTexture = nullptr;
SDL_Texture* targetTexture = nullptr;

void frame ()
{
    SDL_RenderClear (renderer);
    SDL_Rect destinationRectangle {50, 50, 200, 100};
    SDL_SetRenderTarget (renderer, targetTexture);
    SDL_RenderCopy (renderer, imageTexture, nullptr, &destinationRectangle);

    // Copy the target texture to browser window
    SDL_SetRenderTarget (renderer, windowTexture);
    SDL_RenderCopy (renderer, targetTexture, nullptr, nullptr);

    SDL_RenderPresent (renderer);
}

int main ()
{
    SDL_Init (SDL_INIT_VIDEO);
    IMG_Init (IMG_INIT_PNG);

    emscripten_set_canvas_element_size ("#canvas", 800, 600);
    emscripten_set_element_css_size ("#canvas", 800, 600);

    window = SDL_CreateWindow (
        "SDL Emscripten test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_SHOWN
    );

    renderer = SDL_CreateRenderer (
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    // SDL_Surface* surface = IMG_Load ("/resources/11_03 front.png");
    // texture = SDL_CreateTextureFromSurface (renderer, surface);
    // SDL_FreeSurface (surface);
    imageTexture = IMG_LoadTexture (renderer, "/resources/11_03 front.png");

    // Create a target texture twice the canvas size
    targetTexture = SDL_CreateTexture (renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
        1600, 1200);

    emscripten_set_main_loop (frame, 0, 1);
}

#endif
