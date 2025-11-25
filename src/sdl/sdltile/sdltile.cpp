#include "sdltile.h"

#include <SDL_image.h>
#include <stdexcept>

using std::make_pair;
using std::tie;
using std::runtime_error;

SDLTile::SDLTile (string imageFile, SDLRenderer& renderer, 
    SDLTexture& targetTexture, Frame<int> frame)
    :
    sdlRenderer_ {renderer},
    targetTexture_ {targetTexture},
    x_ {frame.x},
    y_ {frame.y},
    width_ {frame.width},
    height_ {frame.height}
{
    // Load image at specified path
    sdlTtexture_ = renderer.createTexture (imageFile);
}

SDLTile::SDLTile (Image& image, SDLRenderer& renderer,
    SDLTexture& targetTexture, Frame<int> frame)
    :
    sdlRenderer_ {renderer},
    targetTexture_ {targetTexture},
    x_ {frame.x},
    y_ {frame.y},
    width_ {frame.width},
    height_ {frame.height}
{
    sdlTtexture_ = renderer.createTexture (image);
}

// Render this tile to the target texture
void SDLTile::render ()
{
    sdlRenderer_.setTarget (targetTexture_);

    // Set rendering space and render texture
    sdlRenderer_.copy (*sdlTtexture_,
        Frame<int> {x_, y_, width_, height_});
}

// Determine if the given position is within the bounds of the tile within
// an optional margin. The margin is expressed as a floating point number
// relative to the dimensions of the texture. A margin of 0.0 expresses
// no margin, a margin of 1.0 indicates the margin equals the width and height
// of the texture.
bool SDLTile::isWithinBounds (Position position, float margin) const
{
    float marginX = margin * width_;
    float marginY = margin * height_;

    if (position.x < x_ - marginX)
    {
        // Too far left
        return false;
    }

    if (position.x > x_ + width_ + marginX)
    {
        // Too far right
        return false;
    }
    
    if (position.y < y_ - marginY)
    {
        // Too high
        return false;
    }
    
    if (position.y > y_ + height_ + marginY)
    {
        // Too low
        return false;
    }

    // Inside rectangle
    return true;
}

bool SDLTile::isRightOfCenter (Position position, float xMargin) const
{
    float marginX = xMargin * width_;
    float marginY = xMargin * width_ / 2;

#if 1
    return position.x > x_ + width_ / 2 &&
        position.x <= x_ + width_ / 2 + marginX;
#else
    return position.x > x_ && position.x <= x_ + marginX &&
        (position.y < y_ && position.y >= y_ - marginY ||
            position.y > y_ && position.y <= y_ + marginY);
#endif
}

bool SDLTile::isLeftOfCenter (Position position, float xMargin) const
{
    float marginX = xMargin * width_;
    float marginY = xMargin * width_ / 2;

#if 1
    return position.x < x_ + width_ / 2 &&
        position.x >= x_ + width_ / 2 - marginX;
#else
    return position.x < x_ && position.x >= x_ - marginX &&
        (position.y < y_ && position.y >= y_ - marginY ||
            position.y > y_ && position.y <= y_ + marginY);
#endif
}

bool SDLTile::isAboveCenter (Position position, float margin) const
{
    float marginY = margin * height_;

    return position.y < y_ + height_ / 2 &&
        position.y >= y_ - marginY &&
        position.x >= x_ && position.x <= x_ + width_;
}

bool SDLTile::isBelowCenter (Position position, float margin) const
{
    float marginY = margin * height_;

    return position.y > y_ + height_ / 2 &&
        position.y <= y_ + height_ + marginY &&
        position.x >= x_ && position.x <= x_ + width_;
}
