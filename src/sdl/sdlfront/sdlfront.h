#ifndef _SDLFRONT_H_
#define _SDLFRONT_H_

#include "panel.h"
#include "../sdltexture/sdltexture.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <SDL.h>

using std::string;
using std::unique_ptr;

class SDLFront : public Front
{
public:
    SDLFront (string imageFile, unique_ptr<SDLRenderer> &sdlRenderer,
        SDL_Texture* targetTexture, Frame<int> frame);
    ~SDLFront ();
    void render ();

private:
    // The texture and renderer to use for this front
    unique_ptr<SDLTexture> sdlTtexture_;
    unique_ptr<SDLRenderer>& sdlRenderer_;
};

#endif // _SDLFRONT_H_