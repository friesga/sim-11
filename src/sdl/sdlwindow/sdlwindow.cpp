#include "sdlwindow.h"

#include <SDL_image.h>
#include <string>

using std::string;
using std::make_unique;

SDLWindow::SDLWindow (char const *title, int x, int y, int width, int height)
{
    // Create window
    sdlWindow_ = SDL_CreateWindow (title, x, y, width, height, SDL_WINDOW_SHOWN);

    if (sdlWindow_ == NULL)
        throw "Window could not be created. SDL error: " +
            string (SDL_GetError ());

    // Create renderer for the given window and set background colour
    sdlRenderer_ = make_unique<SDLRenderer> (sdlWindow_);
    sdlRenderer_->setDrawColor (0, 0, 0, 0);
}

SDLWindow::~SDLWindow ()
{
    SDL_DestroyWindow (sdlWindow_);
}

void SDLWindow::createFront (string imageFile, 
        int x, int y, int width, int height)
{
    fronts_.push_back (make_unique<SDLFront> (imageFile, 
        sdlRenderer_->sdl_Renderer_, x, y, width, height));
}

Indicator *SDLWindow::createIndicator (string imageFile, Indicator::State showFigure,
    int x, int y, int width, int height)
{
    indicators_.push_back (make_unique<SDLIndicator> (imageFile, 
        sdlRenderer_->sdl_Renderer_, showFigure, x, y, width, height));
    return indicators_.back ().get ();
}

// Add a Button to the Window, returning a pointer to the added Button.
//
// It's the user's responsibility not to use this pointer when the Window
// the Button belongs to is destroyed. Not following this instruction will
// lead to undefined behaviour.
//
Button *SDLWindow::createLatchingButton (string buttonDownImage, string buttonUpImage,
    Button::State initialState, Button::EventCallback buttonClicked,
    int x, int y, int width, int height)
{
    buttons_.push_back (make_unique<SDLLatchingButton> (buttonDownImage, buttonUpImage,
        initialState, sdlRenderer_->sdl_Renderer_, buttonClicked, x, y, width, height));
    return buttons_.back ().get ();
}

Button *SDLWindow::createMomentaryButton (string buttonDownImage, string buttonUpImage, 
        Button::State initialState, Button::EventCallback buttonClicked,
        int x, int y, int width, int height)
{
    buttons_.push_back (make_unique<SDLMomentaryButton> (buttonDownImage, buttonUpImage,
        initialState, sdlRenderer_->sdl_Renderer_, buttonClicked, x, y, width, height));
    return buttons_.back ().get ();
}

// Render all Figure's in this window
void SDLWindow::render ()
{
    // Render all fronts, indicators and buttons
    for (auto& sdlFront : fronts_)
        sdlFront->render ();

    for (auto& indicator : indicators_)
        indicator->render ();

    for (auto& button : buttons_)
        button->render ();

    sdlRenderer_->update ();
}

bool SDLWindow::handleEvents ()
{
    SDL_Event event;
	if (SDL_PollEvent (&event))
	{
		if (event.type == SDL_QUIT)
				return true;
        
        for (auto& button : buttons_)
            button->handleEvent (&event);
	}

    return false;
}