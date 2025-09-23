#include "sdltexture.h"

#include <stdexcept>
#include <string>

using std::runtime_error;
using std::string;

// The default constructor creates an empty texture with the given dimensions
SDLTexture::SDLTexture (SDL_Renderer* renderer, int textureWidth, int textureHeight)
{
    sdlTexture_ = SDL_CreateTexture (renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
        textureWidth, textureHeight);

    if (sdlTexture_ == NULL)
        throw runtime_error ("Target texture could not be created. SDL error: " +
            string (SDL_GetError ()));
}

void SDLTexture::setColorModulation (uint8_t red, uint8_t green, uint8_t blue)
{
    SDL_SetTextureColorMod (sdlTexture_, red, green, blue);
}