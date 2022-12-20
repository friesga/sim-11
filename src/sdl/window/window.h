#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <SDL.h>

class Window
{
    // The Renderer class needs access to the SDL_Window to be able to
    // create an SDL_Renderer for this window.
    friend class Renderer;

    SDL_Window* sdlWindow_;

    static int const defaultWindowWidth = 640;
    static int const defaultWindowHeight = 480;

public:
    Window (char const *title,
        int x = SDL_WINDOWPOS_UNDEFINED,
        int y = SDL_WINDOWPOS_UNDEFINED,
        int width = defaultWindowWidth,
        int height = defaultWindowHeight, 
        unsigned long flags = SDL_WINDOW_SHOWN);
    ~Window ();
};


#endif // !_WINDOW_H_
