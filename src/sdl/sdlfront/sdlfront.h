#ifndef _SDLFRONT_H_
#define _SDLFRONT_H_

#include "panel.h"
#include "../sdltile/sdltile.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <SDL.h>
#include <memory>

using std::unique_ptr;

class SDLFront : public Front
{
public:
    SDLFront (unique_ptr<SDLTile> frontTexture);
    ~SDLFront ();
    void render ();

private:
    // The texture to use for this front
    unique_ptr<SDLTile> sdlTtexture_;
};

#endif // _SDLFRONT_H_