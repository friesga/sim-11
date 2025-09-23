#ifndef _SDLTEXTURE_H_
#define _SDLTEXTURE_H_

#include <SDL.h>

class SDLTexture
{
    // Just SDLRenderer can create textures
    friend class SDLRenderer;

public:

#if 0
    // Non-copyable, only movable
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
#endif


private:
    SDLTexture (SDL_Renderer* renderer, int textureWidth, int textureHeight);

    SDL_Texture* sdlTexture_;
};

#endif // _SDLTEXTURE_H_
