#include "sdlpanel.h"
#include "../sdlwindow/sdlwindow.h"

#include <SDL.h>
#include <SDL_image.h>

using std::make_unique;
using std::string;

SDLPanel::SDLPanel ()
{
    // Initialize SDL
    if (SDL_Init (SDL_INIT_VIDEO) < 0)
        throw "SDL could not be initialized SDL error: " +
            string (SDL_GetError ());

    // Set texture filtering to linear
    if (!SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        throw "Linear texture filtering not enabled";

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init (imgFlags) & imgFlags))
        throw "SDL_image could not be initialized. SDL_image error: " +
            string (IMG_GetError ());
}

SDLPanel::~SDLPanel ()
{
    // Quit SDL subsystems
    IMG_Quit ();
    SDL_Quit ();
}

unique_ptr<Window> SDLPanel::createWindow (char const *title, 
    int x, int y, int width, int height)
{
    return make_unique<SDLWindow> (title, x,y, width, height);
}
