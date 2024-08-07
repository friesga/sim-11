#include "sdltexture.h"

#include <SDL_image.h>

using std::make_pair;
using std::tie;

SDLTexture::SDLTexture (string imageFile, SDL_Renderer *renderer, 
    SDL_Texture* targetTexture, Panel::BoundingBox boundingBox)
    :
    sdlRenderer_ {renderer},
    targetTexture_ {targetTexture}
{
    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load (imageFile.c_str ());
    if (loadedSurface == NULL)
        throw "Unable to load image " + imageFile +
            " SDL_image error: " + IMG_GetError ();

    // Color key image
    // SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

    // Create texture from surface pixels
    sdlTtexture_ = SDL_CreateTextureFromSurface (renderer, loadedSurface);
    if (sdlTtexture_ == NULL)
        throw "Unable to create texture from " + imageFile +
            "SDL error: " + SDL_GetError ();

    // Get rid of old loaded surface
    SDL_FreeSurface (loadedSurface);

    // The passed bounding box contains positions relative to the target
    // texture. Calculate the bounding box position and dimensions in pixels
    // from these relative values.
    auto [width, height] = getTextureDimensions (targetTexture_);
    x_ = static_cast<int> (boundingBox.x * width);
    y_ = static_cast<int> (boundingBox.y * height);
    width_ = static_cast<int> (boundingBox.width * width);
    height_ = static_cast<int> (boundingBox.height * height);
}

SDLTexture::~SDLTexture ()
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

// Render this texture to the target texture
void SDLTexture::render ()
{
    if (SDL_SetRenderTarget (sdlRenderer_, targetTexture_) != 0)
        throw "Unable to set render target: " + string (SDL_GetError ());

    // Set rendering space and render texture
    SDL_Rect renderQuad {x_, y_, width_, height_};
    SDL_RenderCopy (sdlRenderer_, sdlTtexture_, NULL, &renderQuad);
}

bool SDLTexture::isWithinBounds (int x, int y) const
{
    if (x < x_)
    {
        // Too far left
        return false;
    }

    if (x > x_ + width_)
    {
        // Too far right
        return false;
    }
    
    if (y < y_)
    {
        // Too high
        return false;
    }
    
    if (y > y + height_)
    {
        // Too low
        return false;
    }

    // Inside rectangle
    return true;
}

pair<int, int> SDLTexture::getTextureDimensions (SDL_Texture* texture)
{
    Uint32 format;
    int access;
    int width, height;

    SDL_QueryTexture (texture, &format, &access, &width, &height);
    return make_pair (width, height);
}
