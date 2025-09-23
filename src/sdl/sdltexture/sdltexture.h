#ifndef _SDLTEXTURE_H_
#define _SDLTEXTURE_H_

#include <SDL.h>
#include <utility>
#include <string>

using std::pair;
using std::string;

class SDLTexture
{
    // Just SDLRenderer can create textures
    friend class SDLRenderer;

public:
    ~SDLTexture ();

    // SDLTextures are non-copyable, only movable
    SDLTexture (const SDLTexture&) = delete;
    SDLTexture& operator= (const SDLTexture&) = delete;
    
    SDLTexture (SDLTexture&& other) noexcept;
    SDLTexture& operator= (SDLTexture&& other) noexcept;

    void setColorModulation (uint8_t red, uint8_t green, uint8_t blue);
    pair<int, int> dimensions () const;

private:
    // Textures can only be created by the SDLRenderer
    SDLTexture (SDL_Renderer* renderer, int textureWidth, int textureHeight);
    SDLTexture (SDL_Renderer* renderer, string imageFile);

    SDL_Texture* sdlTexture_;
};

#endif // _SDLTEXTURE_H_
