#include "sdlinit.h"

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

SDLInit::SDLInit ()
{
    // Initialize SDL
    if (SDL_Init (SDL_INIT_VIDEO) < 0)
        throw runtime_error ("SDL could not be initialized SDL error: " +
            string (SDL_GetError ()));

    // Set texture filtering to linear
    if (!SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        throw runtime_error ("Linear texture filtering not enabled");

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init (imgFlags) & imgFlags))
        throw runtime_error ("SDL_image could not be initialized. SDL_image error: " +
            string (IMG_GetError ()));
}


// Quit SDL subsystems
SDLInit::~SDLInit ()
{
    IMG_Quit ();
    SDL_Quit ();
}