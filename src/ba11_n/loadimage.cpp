#include "ba11_n.h"

#include <SDL_image.h>

SDL_Surface *BA11_N::loadImage (std::string image)
{
    //The final optimized image
    SDL_Surface* optimizedSurface = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load (image.c_str());
    if (loadedSurface == NULL)
        throw std::string("Unable to load image ") + image + 
            std::string("SDL_image Error: ") + IMG_GetError();

    //Convert surface to screen format
    SDL_PixelFormat *windowPixelFormat = 
        SDL_AllocFormat (SDL_GetWindowPixelFormat (window_));
    optimizedSurface = SDL_ConvertSurface (loadedSurface, 
        windowPixelFormat, 0);
    if (optimizedSurface == NULL)
        throw std::string("Unable to optimize image ") + image +
            std::string("SDL Error: ") + SDL_GetError();

    //Get rid of old loaded surface
    SDL_FreeSurface (loadedSurface);

    return optimizedSurface;
}