#ifndef _SDLTEXTURE_H_
#define _SDLTEXTURE_H_

#include <SDL.h>
#include <string>

using std::string;

class SDLTexture
{
public:
    SDLTexture (string imageFile, SDL_Renderer *renderer, 
        SDL_Texture* targetTexture, int x, int y, int width, int height);
    ~SDLTexture ();
    void render ();
    bool isWithinBounds (int x, int y);

private:
    // The actual hardware texture
    SDL_Texture* sdlTtexture_;

    // The SDL renderer to use
    SDL_Renderer *sdlRenderer_;

    // Reference to the target texture to render this texture on
    SDL_Texture* targetTexture_;

protected:
    // Image positon and dimensions
    int x_;
    int y_;
    int width_;
    int height_;
};

#endif // _SDLTEXTURUE_H_