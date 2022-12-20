#include "texture.h"

#include <SDL_image.h>

Texture::Texture (Renderer &renderer, std::string path)
    : 
    renderer_ {renderer}
{
    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load (path.c_str ());
    if (loadedSurface == NULL)
        throw "Unable to load image " + path +
            " SDL_image error: " + IMG_GetError ();

    //Color key image
    // SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

    //Create texture from surface pixels
    sdlTtexture_ = SDL_CreateTextureFromSurface (renderer_.sdlRenderer_, loadedSurface);
    if (sdlTtexture_ == NULL)
        throw "Unable to create texture from " + path +
            "SDL error: " + SDL_GetError ();

    //Get image dimensions
    width_ = loadedSurface->w;
    height_ = loadedSurface->h;

    //Get rid of old loaded surface
    SDL_FreeSurface (loadedSurface);
}

Texture::~Texture ()
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

void Texture::render (int x, int y)
{
    // Use image dimensions as dimensions to be rendered
    render (x, y, width_, height_);
}

void Texture::render (int x, int y, int width, int height)
{
    // Set rendering space and render texture to window
    SDL_Rect renderQuad = { x, y, width, height };
    SDL_RenderCopy (renderer_.sdlRenderer_, sdlTtexture_, NULL, &renderQuad);
}

int Texture::getWidth ()
{
    return width_;
}

int Texture::getHeight ()
{
    return height_;
}
