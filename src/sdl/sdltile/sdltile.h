#ifndef _SDLTILE_H_
#define _SDLTILE_H_

#include "panel.h"
#include "sdl/sdlrenderer/sdlrenderer.h"

#include <SDL.h>
#include <string>
#include <utility>
#include <memory>

using std::string;
using std::pair;
using std::unique_ptr;

class SDLTile
{
public:
    SDLTile (string imageFile, SDLRenderer& renderer, 
        SDL_Texture* targetTexture, Frame<int> frame);

    void render ();
    bool isWithinBounds (Position position, float margin = 0.0) const;
    bool isRightOfCenter (Position position, float margin = 0.0) const;
    bool isLeftOfCenter (Position position, float margin = 0.0) const;

private:
    // The actual hardware texture
    unique_ptr<SDLTexture> sdlTtexture_;

    // The SDL renderer to use
    SDLRenderer& sdlRenderer_;

    // Reference to the target texture to render this texture on
    SDL_Texture* targetTexture_;

protected:
    // Image positon and dimensions
    int x_;
    int y_;
    int width_;
    int height_;
};

#endif // _SDLTILE_H_