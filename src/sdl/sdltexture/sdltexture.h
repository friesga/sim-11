#ifndef _SDLTEXTURE_H_
#define _SDLTEXTURE_H_

#include "panel.h"

#include <SDL.h>
#include <string>
#include <utility>

using std::string;
using std::pair;

class SDLTexture
{
public:
    SDLTexture (string imageFile, SDL_Renderer *renderer, 
        SDL_Texture* targetTexture, Frame<int> frame);
    ~SDLTexture ();
    void render ();
    bool isWithinBounds (Position position, float margin = 0.0) const;

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