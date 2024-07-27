#ifndef _SDLFRONT_H_
#define _SDLFRONT_H_

#include "panel.h"
#include "../sdltexture/sdltexture.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <SDL.h>

using std::string;
using std::unique_ptr;

class SDLFront
{
public:
    SDLFront (string imageFile, unique_ptr<SDLRenderer> &sdlRenderer,
        int x, int y, int width, int height);
    ~SDLFront ();
    void render ();
    void render (SDL_Texture* texture);

private:
    // The texture and renderer to use for this front
    unique_ptr<SDLTexture> sdlTtexture_;
    unique_ptr<SDLRenderer>& sdlRenderer_;

    // Image positon and dimensions
    int x_;
    int y_;
    int width_;
    int height_;
};

#endif // _SDLFRONT_H_