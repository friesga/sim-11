#include "sdlinit.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

SDLInit::SDLInit ()
{
    // Initialize SDL
    if (!SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        throw runtime_error ("SDL could not be initialized SDL error: " +
            string (SDL_GetError ()));

    // Set texture filtering to linear
    // In SDL3 the default texture scale mode is SDL_SCALEMODE_LINEAR.
}


// Quit SDL subsystems
SDLInit::~SDLInit ()
{
    SDL_Quit ();
}