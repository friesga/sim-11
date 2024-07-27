#include "sdlpanel.h"

using std::make_unique;
using std::string;

SDLPanel::SDLPanel (unique_ptr<SDLRenderer> &sdlRenderer)
    :
    sdlRenderer_ {sdlRenderer}
{}

SDLPanel::~SDLPanel ()
{}

void SDLPanel::createFront (string imageFile, 
        int x, int y, int width, int height)
{
    fronts_.push_back (make_unique<SDLFront> (imageFile, 
        sdlRenderer_, x, y, width, height));
}

Indicator *SDLPanel::createIndicator (string imageFile, Indicator::State showFigure,
    int x, int y, int width, int height)
{
    indicators_.push_back (make_unique<SDLIndicator> (imageFile, 
        sdlRenderer_, showFigure, x, y, width, height));
    return indicators_.back ().get ();
}

// Add a Button to the Panel, returning a pointer to the added Button.
//
// It's the user's responsibility not to use this pointer when the Window
// the Panel and Button belong to is destroyed. Not following this instruction
// will lead to undefined behaviour.
//
Button *SDLPanel::createLatchingButton (string buttonDownImage, string buttonUpImage,
    Button::State initialState, Button::EventCallback buttonClicked,
    int x, int y, int width, int height)
{
    buttons_.push_back (make_unique<SDLLatchingButton> (buttonDownImage, buttonUpImage,
        initialState, sdlRenderer_, buttonClicked, x, y, width, height));
    return buttons_.back ().get ();
}

Button *SDLPanel::createMomentaryButton (string buttonDownImage, string buttonUpImage, 
        Button::State initialState, Button::EventCallback buttonClicked,
        int x, int y, int width, int height)
{
    buttons_.push_back (make_unique<SDLMomentaryButton> (buttonDownImage, buttonUpImage,
        initialState, sdlRenderer_, buttonClicked, x, y, width, height));
    return buttons_.back ().get ();
}

// Render all Figure's in this window
void SDLPanel::render ()
{
    // Render all fronts, indicators and buttons
    for (auto& sdlFront : fronts_)
        sdlFront->render ();

    for (auto& indicator : indicators_)
        indicator->render ();

    for (auto& button : buttons_)
        button->render ();
}

void SDLPanel::render (SDL_Texture* texture)
{
    // Render all fronts, indicators and buttons
    for (auto& sdlFront : fronts_)
        sdlFront->render (texture);

    for (auto& indicator : indicators_)
        indicator->render (texture);

    for (auto& button : buttons_)
        button->render (texture);
}

// Events for a Panel are destined for a button on the panel
void SDLPanel::handleEvent (SDL_Event const *event)
{
    for (auto& button : buttons_)
        button->handleEvent (event);
}
