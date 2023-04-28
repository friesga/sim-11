#ifndef _SDLFRONT_H_
#define _SDLFRONT_H_

#include "panel.h"
#include "../sdltexture/sdltexture.h"

#include <SDL.h>

using std::string;
using std::unique_ptr;

class SDLFront
{
public:
    SDLFront (string imageFile, SDL_Renderer *renderer,
        int x, int y, int width, int height);
    ~SDLFront ();
    void render ();

private:
    // The texture to use for this indicator
    unique_ptr<SDLTexture> sdlTtexture_;

    // Image positon and dimensions
    int x_;
    int y_;
    int width_;
    int height_;
};

#endif // _SDLFRONT_H_