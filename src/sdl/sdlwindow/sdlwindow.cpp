#include "sdlwindow.h"
#include "../sdlpanel/sdlpanel.h"
#include "../sdlevent/sdlevent.h"
#include "rackunit.h"

#include <SDL_image.h>
#include <string>
#include <thread>
#include <utility>
#include <algorithm>

using std::string;
using std::make_unique;
using std::this_thread::sleep_for;
using std::pair;
using std::make_pair;
using std::ranges::any_of;

SDLWindow::SDLWindow (char const *title, Frame<int> frame,
    set<Window::Flag> flags)
    :
    SDLInit (),
    windowWidth_ {frame.width},
    windowHeight_ {frame.height},
    textureWidth_ {frame.width * 2},
    textureHeight_ {frame.height * 2}
{
    // Create window
    sdlWindow_ = SDL_CreateWindow (title, frame.x, frame.y, frame.width, frame.height,
        flags.contains (Window::Flag::WindowHidden) ? SDL_WINDOW_HIDDEN : SDL_WINDOW_SHOWN);

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

void SDLWindow::show ()
{
    SDL_ShowWindow (sdlWindow_);
}

Panel *SDLWindow::createPanel (Cabinet::Position cabinetPosition,
    RackUnit unitHeight)
{
    panels_.push_back (make_unique<SDLPanel> (sdlRenderer_, targetTexture_,
        cabinetPosition, unitHeight));
    return panels_.back ().get ();
}

void SDLWindow::render ()
{    
    // Render all Panels to the target texture
    for (auto& sdlPanel : panels_)
        sdlPanel->render ();

    // Copy the target texture to the window frame buffer
    sdlRenderer_->copy (targetTexture_);

    if (showLoupe_)
        drawLoupe ();
    else
        SDL_SetTextureColorMod (targetTexture_, 255, 255, 255);

    sdlRenderer_->update ();
}

void SDLWindow::drawLoupe ()
{
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

bool SDLWindow::handleEvents ()
{
    SDL_Event event;
	if (SDL_PollEvent (&event))
	{
        windowPosition_ = {event.button.x, event.button.y};
        texturePosition_ = windowToTexturePosition (windowPosition_);

        switch (event.type)
        {
            case SDL_QUIT:
                return true;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    SDLEvent sdlEvent (&event, texturePosition_);

                    for (auto& sdlPanel : panels_)
                        sdlPanel->handleEvent (&sdlEvent);
                }
                break;

            case SDL_MOUSEMOTION:
                showLoupe_ = any_of (panels_, [&] (unique_ptr<SDLPanel>& p)
                    { return p->isOverButton (texturePosition_); });
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
