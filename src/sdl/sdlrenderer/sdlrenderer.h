#ifndef _SDLRENDERER_H_
#define _SDLRENDERER_H_

#include "panel.h"
#include "sdl/sdltexture/sdltexture.h"

#include <SDL.h>
#include <memory>
#include <string>

using std::unique_ptr;
using std::string;

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

    // The renderer also is factory for textures. In SDL the relationship
    // between textures and renderer is bidirectional. This create issues
    // in a C++ wrapper and to avoid this we create just references from
    // SDLRenderer to the textures using this renderer.
    unique_ptr<SDLTexture> createTexture (int textureWidth, int textureHeight);
    unique_ptr<SDLTexture> createTexture (string imageFile);

    void setTarget (SDLTexture& texture);
    void setDrawColor (unsigned char red, unsigned char green,
        unsigned char blue, unsigned char alpha = 0xff);
    void clear ();
    void copy (SDLTexture& texture);
    void copy (SDLTexture& texture, Frame<int> renderFrame);
    void update ();

    void copyCircle (SDLTexture& texture, Position sourceCenter,
        int sourceRadius, Position destCenter, int destRadius);
    int drawCircle (Position position, int radius);

    SDL_Renderer *getSDL_Renderer ();
};

#endif // _SDLRENDERER_H_