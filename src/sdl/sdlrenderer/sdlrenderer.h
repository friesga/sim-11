#ifndef _SDLRENDERER_H_
#define _SDLRENDERER_H_

#include <SDL.h>

class SDLRenderer
{
    friend class SDLPanel;

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
    void copy (SDL_Texture* texture);
    void update ();
    SDL_Renderer *getSDL_Renderer ();
};

#endif // _SDLRENDERER_H_