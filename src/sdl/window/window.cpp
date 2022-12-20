#include "window.h"

#include <string>

// Constructor
Window::Window (char const *title, int x, int y, 
    int width, int height, unsigned long flags)
{
    // Create window
    sdlWindow_ = SDL_CreateWindow (title, x, y, width, height, flags);

    if (sdlWindow_ == NULL)
        throw "Window could not be created. SDL error: " +
        std::string (SDL_GetError ());
}

// Destructor
Window::~Window ()
{
    SDL_DestroyWindow (sdlWindow_);
}