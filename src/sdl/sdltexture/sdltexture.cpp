#include "sdltexture.h"

#include <SDL_image.h>
#include <stdexcept>
#include <string>

using std::runtime_error;
using std::string;
using std::pair;
using std::make_pair;

// The default constructor creates an empty texture with the given dimensions
SDLTexture::SDLTexture (SDL_Renderer* renderer, int textureWidth, int textureHeight)
{
    sdl2_Texture_ = SDL_CreateTexture (renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
        textureWidth, textureHeight);

    if (sdl2_Texture_ == NULL)
        throw runtime_error ("Target texture could not be created. SDL error: " +
            string (SDL_GetError ()));
}

SDLTexture::SDLTexture (SDL_Renderer* renderer, string imageFile)
{
    // Load image at specified path
    sdl2_Texture_ = IMG_LoadTexture (renderer, imageFile.c_str ());

    if (sdl2_Texture_ == NULL)
        throw runtime_error ("Unable to create texture from " + imageFile +
            " SDL error: " + SDL_GetError ());
}

SDLTexture::SDLTexture (SDL_Renderer* renderer, Image& image)
{
    SDL_RWops* rw = SDL_RWFromConstMem (image.data ().data (),
        image.data ().size ());
    sdl2_Texture_ = IMG_LoadTexture_RW (renderer, rw, 1);
}

SDLTexture::~SDLTexture ()
{
    // Deallocate texture if allocated
    if (sdl2_Texture_ != NULL)
    {
        SDL_DestroyTexture (sdl2_Texture_);
        sdl2_Texture_ = NULL;
    }
}

SDLTexture::SDLTexture (SDLTexture&& other) noexcept
    : 
    sdl2_Texture_ (other.sdl2_Texture_)
{
    other.sdl2_Texture_ = nullptr;
}

SDLTexture& SDLTexture::operator= (SDLTexture&& other) noexcept
{
    if (this != &other)
    {
        if (sdl2_Texture_)
            SDL_DestroyTexture (sdl2_Texture_);

        sdl2_Texture_ = other.sdl2_Texture_;
        other.sdl2_Texture_ = nullptr;
    }
    return *this;
}

void SDLTexture::setColorModulation (uint8_t red, uint8_t green, uint8_t blue)
{
    SDL_SetTextureColorMod (sdl2_Texture_, red, green, blue);
}

pair<int, int> SDLTexture::dimensions () const
{
    Uint32 format;
    int access;
    int width, height;

    SDL_QueryTexture (sdl2_Texture_, &format, &access, &width, &height);
    return make_pair (width, height);
}
