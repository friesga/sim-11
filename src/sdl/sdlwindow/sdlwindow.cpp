#include "sdlwindow.h"
#include "../sdlpanel/sdlpanel.h"
#include "../sdlevent/sdlevent.h"

#include <SDL_image.h>
#include <string>
#include <thread>
#include <utility>

using std::string;
using std::make_unique;
using std::this_thread::sleep_for;
using std::pair;
using std::make_pair;

SDLWindow::SDLWindow (char const *title, int x, int y, int width, int height)
    :
    SDLInit (),
    windowWidth_ {width},
    windowHeight_ {height},
    textureWidth_ {width},
    textureHeight_ {height}
{
    // Create window
    sdlWindow_ = SDL_CreateWindow (title, x, y, width, height, SDL_WINDOW_SHOWN);

    if (sdlWindow_ == NULL)
        throw "Window could not be created. SDL error: " +
            string (SDL_GetError ());

    // Create renderer for the given window and set background colour
    sdlRenderer_ = make_unique<SDLRenderer> (sdlWindow_);
    sdlRenderer_->setDrawColor (0, 0, 0, 0);

    // ToDo: Make target texture large enough to be able to zoom into it
    targetTexture_ = SDL_CreateTexture (sdlRenderer_->getSDL_Renderer (),
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
        textureWidth_, textureHeight_);

    if (targetTexture_ == NULL)
        throw "Target texture could not be created. SDL error: " +
            string (SDL_GetError ());
}

SDLWindow::~SDLWindow ()
{
    SDL_DestroyWindow (sdlWindow_);
}

Panel *SDLWindow::createPanel ()
{
    panels_.push_back (make_unique<SDLPanel> (sdlRenderer_, targetTexture_));
    return panels_.back ().get ();
}

void SDLWindow::render ()
{    
    // Render all Panels in the window
    for (auto& sdlPanel : panels_)
        sdlPanel->render ();

    sdlRenderer_->copy (targetTexture_);
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
        {
            auto [x, y] = windowToTexturePosition (event.button.x, event.button.y);
            SDLEvent sdlEvent (&event, x, y);
            sdlPanel->handleEvent (&sdlEvent);
        }
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

// This function transforms a position in this window (as specified in
// e.g. a mouse button event) to a position in the target texture.
pair<int, int> SDLWindow::windowToTexturePosition (int windowPositionX,
    int windowPositionY)
{
    return {windowPositionX * textureWidth_ / windowWidth_,
        windowPositionY * textureHeight_ / windowHeight_};
}