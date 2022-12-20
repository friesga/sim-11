#include "sdlwrap.h"

#include <SDL.h>
#include <SDL_image.h>
#include <string>

SDLwrap::SDLwrap ()
{
    //Initialize SDL
    if (SDL_Init (SDL_INIT_VIDEO) < 0)
        throw "SDL could not be initialized SDL error: " +
        std::string (SDL_GetError ());

    //Set texture filtering to linear
    if (!SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        throw "Linear texture filtering not enabled";

    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init (imgFlags) & imgFlags))
        throw "SDL_image could not be initialized. SDL_image error: " +
        std::string (IMG_GetError ());
}

SDLwrap::~SDLwrap ()
{
    //Quit SDL subsystems
    IMG_Quit ();
    SDL_Quit ();
}