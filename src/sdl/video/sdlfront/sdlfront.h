#ifndef _SDLFRONT_H_
#define _SDLFRONT_H_

#include "panel.h"
#include "../sdltile/sdltile.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <SDL3/SDL.h>
#include <memory>
#include <array>

class SDLFront : public Front
{
public:
    SDLFront (unique_ptr<SDLTile> primaryTile, 
        EventHandler eventHandler = nullptr);
    SDLFront (unique_ptr<SDLTile> primaryTile,
        unique_ptr<SDLTile> secondaryTile,
        State initialState, EventHandler eventHandler);
    ~SDLFront ();
    void show (State image) override;
    void render () override;
    void handleEvent (InputEvent const* event) override;

private:
    // The tiles to be used for this front
    std::array<unique_ptr<SDLTile>, 2> frontTiles_;

    State state_ {State::Primary};
    EventHandler eventHandler_;

    bool isWithinBounds (Position position, float margin) const;
};

#endif // _SDLFRONT_H_