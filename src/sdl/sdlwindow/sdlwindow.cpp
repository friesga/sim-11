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
    textureWidth_ {width * 2},
    textureHeight_ {height * 2}
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
    // Render all Panels to the target texture
    for (auto& sdlPanel : panels_)
        sdlPanel->render ();

    // Copy the target texture to the window frame buffer
    sdlRenderer_->copy (targetTexture_);

    if (loupeShown_)
    {
        SDL_SetTextureColorMod (targetTexture_, 128, 128, 128);

        // Draw loupe with enlarged view.
        // The source rectangle is related to a position in the texture
        // while the destination rectangle is related to a postion in
        // the window.
        RenderCopyCircle (sdlRenderer_->getSDL_Renderer (), targetTexture_,
            texturePosition_, loupeRadius_ / 2,
            windowPosition_, loupeRadius_);

        // Draw loupe outline
        SDL_SetRenderDrawColor (sdlRenderer_->getSDL_Renderer (),
            255, 0, 0, 255);
        RenderDrawCircle (sdlRenderer_->getSDL_Renderer (), windowPosition_,
            loupeRadius_);
    }
    else
        SDL_SetTextureColorMod (targetTexture_, 255, 255, 255);


    sdlRenderer_->update ();
}

bool SDLWindow::handleEvents ()
{
    SDL_Event event;
	if (SDL_PollEvent (&event))
	{
        windowPosition_ = {event.button.x, event.button.y};
        texturePosition_ = windowToTexturePosition (windowPosition_);

        for (auto& sdlPanel : panels_)
        {
            SDLEvent sdlEvent (&event, texturePosition_);
            sdlPanel->handleEvent (&sdlEvent);
        }

        switch (event.type)
        {
            case SDL_QUIT:
                return true;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                    loupeShown_ = !loupeShown_;
                break;

            case SDL_MOUSEMOTION:
                break;

            default:
                // Ignore other events
                break;
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
Position SDLWindow::windowToTexturePosition (Position windowPosition)
{
    return {windowPosition.x * textureWidth_ / windowWidth_,
        windowPosition.y * textureHeight_ / windowHeight_};
}
