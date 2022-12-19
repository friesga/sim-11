#include "ba11_n.h"

void BA11_N::init (char const* title, int xpos, int ypos,
    int width, int height, bool fullscreen)
{
    // Define background color
    static const int backGroundRed = 0;
    static const int backGroundGreen = 0;
    static const int backGroundBlue = 0;
    static const int backGroundAlpha = 255;

    // Initialize SDL
    if (SDL_Init (SDL_INIT_EVERYTHING) != 0)
        throw std::string ("SDL initialization failed");

    // Create a unique_ptr to a created window
    window_ = SDL_CreateWindow (title, xpos, ypos, width, height, 
        fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

    if (window_ == nullptr)
        throw std::string ("Window initialization failed");

    // Create a unique_ptr to a created renderer
    renderer_ = SDL_CreateRenderer (window_, -1, 0);

    if (renderer_ == nullptr)
        throw std::string ("Renderer initialization failed");

    // Draw image on a black background
    SDL_SetRenderDrawColor (renderer_, backGroundRed, backGroundGreen,
        backGroundBlue, backGroundAlpha);

    // The image has to be placed in the direcory out/assets.
    SDL_Surface *tempSurface = loadImage ("../../assets/pdp11-03-front.png");
    texture_ = SDL_CreateTextureFromSurface (renderer_, tempSurface);
    SDL_FreeSurface (tempSurface);

    if (texture_ == nullptr)
        throw std::string ("Loading image failed");

    SDL_QueryTexture (texture_, NULL, NULL, 
        &sourceRectangle_.w, &sourceRectangle_.h);

    sourceRectangle_.x = 0;
    sourceRectangle_.y = 0;
    destinationRectangle_.x = 10;
    destinationRectangle_.y = 10;
    destinationRectangle_.w = width;
    destinationRectangle_.h = height;
}