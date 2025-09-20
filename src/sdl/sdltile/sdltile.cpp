#include "sdltile.h"

#include <SDL_image.h>
#include <stdexcept>

using std::make_pair;
using std::tie;
using std::runtime_error;

SDLTile::SDLTile (string imageFile, SDL_Renderer *renderer, 
    SDL_Texture* targetTexture, Frame<int> frame)
    :
    sdlRenderer_ {renderer},
    targetTexture_ {targetTexture},
    x_ {frame.x},
    y_ {frame.y},
    width_ {frame.width},
    height_ {frame.height}
{
    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load (imageFile.c_str ());
    if (loadedSurface == NULL)
        throw runtime_error ("Unable to load image " + imageFile +
            " SDL_image error: " + IMG_GetError ());

    // Create texture from surface pixels
    sdlTtexture_ = SDL_CreateTextureFromSurface (renderer, loadedSurface);
    if (sdlTtexture_ == NULL)
        throw runtime_error ("Unable to create texture from " + imageFile +
            "SDL error: " + SDL_GetError ());

    // Get rid of old loaded surface
    SDL_FreeSurface (loadedSurface);
}

SDLTile::~SDLTile ()
{
    // Deallocate texture if allocated
    if (sdlTtexture_ != NULL)
    {
        SDL_DestroyTexture (sdlTtexture_);
        sdlTtexture_ = NULL;
        width_ = 0;
        height_ = 0;
    }
}

// Render this tile to the target texture
void SDLTile::render ()
{
    if (SDL_SetRenderTarget (sdlRenderer_, targetTexture_) != 0)
        throw runtime_error ("Unable to set render target: " +
            string (SDL_GetError ()));

    // Set rendering space and render texture
    SDL_Rect renderQuad {x_, y_, width_, height_};
    SDL_RenderCopy (sdlRenderer_, sdlTtexture_, NULL, &renderQuad);
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

bool SDLTile::isRightOfCenter (Position position, float margin) const
{
    float marginX = margin * width_;

    return position.x > x_ + width_ / 2 &&
        position.x <= x_ + width_ / 2 + marginX;
}

bool SDLTile::isLeftOfCenter (Position position, float margin) const
{
    float marginX = margin * width_;

    return position.x < x_ + width_ / 2 &&
        position.x >= x_ + width_ / 2 - marginX;
}
