#include "sdltexture.h"

#include <SDL_image.h>

SDLTexture::SDLTexture (string imageFile, SDL_Renderer *renderer, 
    int x, int y, int width, int height)
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

    // Get image dimensions
    // A width and height of value 0 indicates that the width and height of
    // the image has to be used.
    if (width == 0 && height == 0)
    {
        width_ = loadedSurface->w;
        height_ = loadedSurface->h;
    }
    else
    {
        width_ = width;
        height_ = height;
    }

    // Get rid of old loaded surface
    SDL_FreeSurface (loadedSurface);

    // Save figure position and renderer to use in rendering
    // the texture
    x_ = x;
    y_ = y;
    sdlRenderer_ = renderer;
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

void SDLTexture::render ()
{
    // Set rendering space and render texture to window
    SDL_Rect renderQuad {x_, y_, width_, height_};
    SDL_RenderCopy (sdlRenderer_, sdlTtexture_, NULL, &renderQuad);
}

bool SDLTexture::isWithinBounds (int x, int y)
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
