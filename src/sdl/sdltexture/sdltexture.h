#ifndef _SDLTEXTURE_H_
#define _SDLTEXTURE_H_

#include <SDL.h>
#include <utility>

using std::pair;

class SDLTexture
{
    // Just SDLRenderer can create textures
    friend class SDLRenderer;

public:

    // SDLTextures are non-copyable, only movable
    SDLTexture (const SDLTexture&) = delete;
    SDLTexture& operator=(const SDLTexture&) = delete;
    SDLTexture (SDLTexture&& other) noexcept : sdlTexture_ (other.sdlTexture_)
    {
        other.sdlTexture_ = nullptr;
    }
    SDLTexture& operator=(SDLTexture&& other) noexcept
    {
        if (this != &other)
        {
            if (sdlTexture_) SDL_DestroyTexture (sdlTexture_);
            sdlTexture_ = other.sdlTexture_;
            other.sdlTexture_ = nullptr;
        }
        return *this;
    }

    void setColorModulation (uint8_t red, uint8_t green, uint8_t blue);
    pair<int, int> dimensions () const;

    // ToDo: Remove this function
    SDL_Texture* getSDL_Texture () const { return sdlTexture_; }

private:
    SDLTexture (SDL_Renderer* renderer, int textureWidth, int textureHeight);

    SDL_Texture* sdlTexture_;
};

#endif // _SDLTEXTURE_H_
