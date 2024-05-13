#include "sdlwindow.h"
#include "../sdlpanel/sdlpanel.h"

#include <SDL_image.h>
#include <string>
#include <thread>

using std::string;
using std::make_unique;
using std::this_thread::sleep_for;

SDLWindow::SDLWindow (char const *title, int x, int y, int width, int height)
    :
    SDLInit ()
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

Panel *SDLWindow::createPanel ()
{
    panels_.push_back (make_unique<SDLPanel> (sdlRenderer_));
    return panels_.back ().get ();
}

void SDLWindow::render ()
{    
    // Render all Panels in the window
    for (auto& sdlPanel : panels_)
        sdlPanel->render ();

    sdlRenderer_->update ();
}

bool SDLWindow::handleEvents ()
{
    SDL_Event event;
	if (SDL_PollEvent (&event))
	{
		if (event.type == SDL_QUIT)
				return true;
        
        for (auto& sdlPanel : panels_)
            sdlPanel->handleEvent (&event);
	}

    return false;
}

// This function renders the window and process the events for the window. The
// function returns when the window is closed.
void SDLWindow::handler ()
{
    do
	{
		render();
        sleep_for (std::chrono::milliseconds (10));
	}
    while (!handleEvents ());
}
