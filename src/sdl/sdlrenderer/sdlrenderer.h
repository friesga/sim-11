#ifndef _SDLRENDERER_H_
#define _SDLRENDERER_H_

#include <SDL.h>

class SDLRenderer
{
    // SDLFigure needs access to the SDL_Renderer
    friend class SDLWindow;
    friend class SDLFigure;

    // The SDL renderer to use in rendering in the given window
    SDL_Renderer *sdl_Renderer_;

public:
    SDLRenderer (SDL_Window *sdlWindow, 
        int index = -1, 
        unsigned long flags = SDL_RENDERER_ACCELERATED);
    ~SDLRenderer ();

    void setDrawColor (unsigned char red, unsigned char green,
        unsigned char blue, unsigned char alpha = 0xff);
    void clear ();
    void update ();
};

#endif // _SDLRENDERER_H_