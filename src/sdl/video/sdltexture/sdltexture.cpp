#include "sdltexture.h"

#include <SDL3_image/SDL_image.h>
#include <stdexcept>
#include <string>
#include <cmath>   

using std::runtime_error;
using std::string;
using std::pair;
using std::make_pair;
using std::round;

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
    SDL_IOStream* rw = SDL_IOFromConstMem (image.data ().data (),
        image.data ().size ());
    sdl2_Texture_ = IMG_LoadTexture_IO (renderer, rw, true);
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
    float width, height;

    SDL_GetTextureSize (sdl2_Texture_, &width, &height);
    return make_pair (round (width), round (height));
}
