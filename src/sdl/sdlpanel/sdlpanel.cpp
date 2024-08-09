#include "sdlpanel.h"

using std::make_unique;
using std::string;

SDLPanel::SDLPanel (unique_ptr<SDLRenderer> &sdlRenderer,
    SDL_Texture* texture)
    :
    sdlRenderer_ {sdlRenderer},
    targetTexture_ {texture}
{}

SDLPanel::~SDLPanel ()
{}

void SDLPanel::createFront (string imageFile, 
        Panel::BoundingBox boundingBox)
{
    fronts_.push_back (make_unique<SDLFront> (imageFile, 
        sdlRenderer_, targetTexture_, boundingBox));
}

Indicator *SDLPanel::createIndicator (string imageFile,
    Indicator::State showFigure, Panel::BoundingBox boundingBox)
{
    indicators_.push_back (make_unique<SDLIndicator> (imageFile, 
        sdlRenderer_, showFigure, targetTexture_, boundingBox));
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
    BoundingBox boundingBox)
{
    buttons_.push_back (make_unique<SDLLatchingButton> (buttonDownImage, buttonUpImage,
        initialState, sdlRenderer_, buttonClicked, targetTexture_, boundingBox));
    return buttons_.back ().get ();
}

Button *SDLPanel::createMomentaryButton (string buttonDownImage, string buttonUpImage, 
    Button::State initialState, Button::EventCallback buttonClicked,
    BoundingBox boundingBox)
{
    buttons_.push_back (make_unique<SDLMomentaryButton> (buttonDownImage, buttonUpImage,
        initialState, sdlRenderer_, buttonClicked, targetTexture_, boundingBox));
    return buttons_.back ().get ();
}

// Render all elements in this panel to the window
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

// Events for a Panel are destined for a button on the panel
void SDLPanel::handleEvent (SDLEvent const *event)
{
    for (auto& button : buttons_)
        button->handleEvent (event);
}
