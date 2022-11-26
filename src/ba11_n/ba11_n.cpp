//
// Support for the BA11-N Mounting Box
//
#include <iostream>
#include <thread>

#include "ba11_n.h"

// Constructor
// Create a window showing the BA11-N and devices and then start a thread
// handling the events and render the lamps and switches.
BA11_N::BA11_N (char const* title, int xpos, int ypos,
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

    SDL_Surface *tempSurface = loadImage ("assets/pdp11-03-front.png");
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
    destinationRectangle_.w = sourceRectangle_.w;
    destinationRectangle_.h = sourceRectangle_.h;

    running_ = true;

    // Create a thread running the event handler and rendering
    this->ba11_nThread = std::thread (&BA11_N::bezel, this);
}

// Destructor
BA11_N::~BA11_N ()
{
    SDL_DestroyWindow (window_);
    SDL_DestroyRenderer (renderer_);
    SDL_DestroyTexture (texture_);
    SDL_Quit ();
}

void BA11_N::bezel ()
{
    while (running_)
	{
		handleEvents();
		// update();
		render();
        std::this_thread::sleep_for (std::chrono::milliseconds (33));
	}
}

inline bool BA11_N::isRunning ()
{
    return running_;
}
