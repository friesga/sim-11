#ifndef _SDLGRAPHICSCONTEXT_H_
#define _SDLGRAPHICSCONTEXT_H_

#include "panel.h"

#include <SDL.h>

// For SDL the graphics context comprises the renderer and the target texture.
//
class SDLGraphicsContext : public GraphicsContext
{
public:
    SDLGraphicsContext (SDL_Renderer* renderer, SDL_Texture* targetTexture);

    // Function required by the GraphicsContext interface
    virtual void setAsTarget () override;

private:
    // The SDL renderer to use
    SDL_Renderer* sdlRenderer_;

    // Reference to the target texture to render this texture on
    SDL_Texture* targetTexture_;
};

#endif  // _SDLGRAPHICSCONTEXT_H_