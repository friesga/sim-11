#ifndef _SDLFRONT_H_
#define _SDLFRONT_H_

#include "panel.h"
#include "../sdltile/sdltile.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <SDL3/SDL.h>
#include <memory>

class SDLFront : public Front
{
public:
    SDLFront (unique_ptr<SDLTile> frontTile);
    ~SDLFront ();
    void render () override;
    void handleEvent (InputEvent const* event);

private:
    // The tile to use for this front
    std::unique_ptr<SDLTile> frontTile_;
};

#endif // _SDLFRONT_H_