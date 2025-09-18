#include "sdlgraphicscontext.h"

#include <stdexcept>

using std::runtime_error;

SDLGraphicsContext::SDLGraphicsContext(SDL_Renderer* renderer,
    SDL_Texture* targetTexture)
    :
    sdlRenderer_ {renderer},
    targetTexture_ {targetTexture}
{}

void SDLGraphicsContext::setAsTarget ()
{
    if (SDL_SetRenderTarget (sdlRenderer_, targetTexture_) != 0)
        throw runtime_error ("Unable to set render target: " +
            string (SDL_GetError ()));
}