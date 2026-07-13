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
    SDLFront (unique_ptr<SDLTile> frontTile);
    ~SDLFront ();
    void render () override;

private:
    // The tile to use for this front
    unique_ptr<SDLTile> frontTile_;
};

#endif // _SDLFRONT_H_