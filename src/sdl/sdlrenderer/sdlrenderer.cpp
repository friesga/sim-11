#include "sdlrenderer.h"
#include "../sdlwindow/sdlwindow.h"

#include <string>

SDLRenderer::SDLRenderer (SDL_Window *sdlWindow, int index, unsigned long flags)
{
    // Create renderer for the given window
    sdl_Renderer_ = SDL_CreateRenderer (sdlWindow, index, flags);

    if (sdl_Renderer_ == NULL)
        throw "Renderer could not be created. SDL error: " +
        std::string (SDL_GetError ());
}

SDLRenderer::~SDLRenderer ()
{
    SDL_DestroyRenderer (sdl_Renderer_);
}

void SDLRenderer::setDrawColor (unsigned char red, unsigned char green,
        unsigned char blue, unsigned char alpha)
{
    SDL_SetRenderDrawColor (sdl_Renderer_, red, green, blue, alpha);
}

void SDLRenderer::clear ()
{
    SDL_RenderClear (sdl_Renderer_);
}

void SDLRenderer::update ()
{
    SDL_RenderPresent (sdl_Renderer_);
}

SDL_Renderer *SDLRenderer::getSDL_Renderer ()
{
    return sdl_Renderer_;
}