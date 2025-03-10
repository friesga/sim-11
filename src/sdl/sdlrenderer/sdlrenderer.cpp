#include "sdlrenderer.h"
#include "../sdlwindow/sdlwindow.h"

#include <stdexcept>
#include <string>

using std::runtime_error;
using std::string;

SDLRenderer::SDLRenderer (SDL_Window *sdlWindow, int index, unsigned long flags)
{
    // Create renderer for the given window
    sdl_Renderer_ = SDL_CreateRenderer (sdlWindow, index, flags);

    if (sdl_Renderer_ == NULL)
        throw runtime_error ("Renderer could not be created. SDL error: " +
            string (SDL_GetError ()));
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

// Draw (copy/render) the given texture to the frame buffer of the window
void SDLRenderer::copy (SDL_Texture* texture)
{
    SDL_SetRenderTarget (sdl_Renderer_, nullptr);
    SDL_RenderCopy (sdl_Renderer_, texture, NULL, NULL);
}

void SDLRenderer::update ()
{
    SDL_RenderPresent (sdl_Renderer_);
}

SDL_Renderer *SDLRenderer::getSDL_Renderer ()
{
    return sdl_Renderer_;
}