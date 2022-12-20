#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../window/window.h"

class Renderer
{
    // The Texture class needs access to the SDL_Renderer to be able to
    // render the texture.
    friend class Texture;

    // The SDL renderer to use in rendering in the given window
    SDL_Renderer *sdlRenderer_;

public:
    Renderer (Window &window, 
        int index = -1, 
        unsigned long flags = SDL_RENDERER_ACCELERATED);
    ~Renderer ();

    void setDrawColor (unsigned char red, unsigned char green,
        unsigned char blue, unsigned char alpha = 0xff);
    void clear ();
    void update ();
};

#endif // _RENDERER_H_