#include "renderer.h"

#include <string>

Renderer::Renderer (Window &window, int index, unsigned long flags)
{
    //Create renderer for the given window
    sdlRenderer_ = SDL_CreateRenderer (window.sdlWindow_, index, flags);

    if (sdlRenderer_ == NULL)
        throw "Renderer could not be created. SDL error: " +
        std::string (SDL_GetError ());
}

Renderer::~Renderer ()
{
    SDL_DestroyRenderer (sdlRenderer_);
}

void Renderer::setDrawColor (unsigned char red, unsigned char green,
        unsigned char blue, unsigned char alpha)
{
    SDL_SetRenderDrawColor (sdlRenderer_, red, green, blue, alpha);
}

void Renderer::clear ()
{
    SDL_RenderClear (sdlRenderer_);
}

void Renderer::update ()
{
    SDL_RenderPresent (sdlRenderer_);
}