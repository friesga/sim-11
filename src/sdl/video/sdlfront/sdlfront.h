#ifndef _SDLFRONT_H_
#define _SDLFRONT_H_

#include "panel.h"
#include "../sdltile/sdltile.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <SDL3/SDL.h>
#include <memory>
#include <functional>

class SDLFront : public Front
{
public:
    using EventHandler = std::function<void (InputEvent const* event)>;

    SDLFront (unique_ptr<SDLTile> frontTile, EventHandler eventHandler = nullptr);
    ~SDLFront ();
    void render () override;
    void handleEvent (InputEvent const* event);

private:
    // The tile to use for this front
    std::unique_ptr<SDLTile> frontTile_;
    EventHandler eventHandler_;
};

#endif // _SDLFRONT_H_