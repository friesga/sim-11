#ifndef _SDLTILE_H_
#define _SDLTILE_H_

#include "panel.h"
#include "sdl/video/sdlrenderer/sdlrenderer.h"
#include "imagedata/include/image.h"

#include <SDL3/SDL.h>
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
        SDLTexture& targetTexture, Frame<int> frame);
    SDLTile (Image& image, SDLRenderer& renderer,
        SDLTexture& targetTexture, Frame<int> frame);

    void render ();
    bool isWithinBounds (Position position, float margin = 0.0) const;
    bool isRightOfCenter (Position position, float xMargin = 0.0) const;
    bool isLeftOfCenter (Position position, float xMargin = 0.0) const;
    bool isAboveCenter (Position position, float xMargin = 0.0) const;
    bool isBelowCenter (Position position, float xMargin = 0.0) const;

private:
    // The actual hardware texture
    unique_ptr<SDLTexture> sdlTtexture_;

    // The SDL renderer to use
    SDLRenderer& sdlRenderer_;

    // Reference to the target texture to render this texture on
    SDLTexture& targetTexture_;

    // Image positon and dimensions
    int x_;
    int y_;
    int width_;
    int height_;

    constexpr int centreY () const { return y_ + height_ / 2; }
    constexpr int centreX () const { return x_ + width_ / 2; }
};

#endif // _SDLTILE_H_