#include "sdlinit.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

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

    if (!TTF_Init ())
    {
       throw runtime_error ("TTF could not be initialized SDL error: " +
            string (SDL_GetError ()));
    }
}


// Quit SDL subsystems
SDLInit::~SDLInit ()
{
    SDL_Quit ();
}