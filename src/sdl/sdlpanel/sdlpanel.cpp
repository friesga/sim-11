#include "sdlpanel.h"

#include <algorithm>
#include <utility>

using std::make_unique;
using std::string;
using std::ranges::find_if;
using std::pair;
using std::make_pair;

SDLPanel::SDLPanel (unique_ptr<SDLRenderer> &sdlRenderer,
    SDL_Texture* texture)
    :
    sdlRenderer_ {sdlRenderer},
    targetTexture_ {texture}
{}

SDLPanel::~SDLPanel ()
{}

void SDLPanel::createFront (string imageFile, 
        Panel::Frame<float> frame)
{
    fronts_.push_back (make_unique<SDLFront> (imageFile, 
        sdlRenderer_, targetTexture_, placeFrameInTexture (frame)));
}

Indicator *SDLPanel::createIndicator (string imageFile,
    Indicator::State showFigure, Panel::Frame<float> frame)
{
    indicators_.push_back (make_unique<SDLIndicator> (imageFile, 
        sdlRenderer_, showFigure, targetTexture_, placeFrameInTexture (frame)));
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
    Frame<float> frame)
{
    buttons_.push_back (make_unique<SDLLatchingButton> (buttonDownImage, buttonUpImage,
        initialState, sdlRenderer_, buttonClicked, targetTexture_,
        placeFrameInTexture (frame)));
    return buttons_.back ().get ();
}

Button *SDLPanel::createMomentaryButton (string buttonDownImage, string buttonUpImage, 
    Button::State initialState, Button::EventCallback buttonClicked,
    Frame<float> frame)
{
    buttons_.push_back (make_unique<SDLMomentaryButton> (buttonDownImage, buttonUpImage,
        initialState, sdlRenderer_, buttonClicked, targetTexture_,
        placeFrameInTexture (frame)));
    return buttons_.back ().get ();
}

// Place the given frame, whith positions and dimensions relative to the
// panel, in the target texture. The given panel frame has position and
// dimensions as fractions relative to the panel dimensions; the target
// texture frame has absolute values.
Panel::Frame<int> SDLPanel::placeFrameInTexture (Panel::Frame<float> frame)
{
    // The passed frame contains positions relative to the target
    // texture. Calculate the bounding box position and dimensions in pixels
    // from these relative values.
    auto [width, height] = getTextureDimensions (targetTexture_);
    int x = static_cast<int> (frame.x * width);
    int y = static_cast<int> (frame.y * height);
    int width_ = static_cast<int> (frame.width * width);
    int height_ = static_cast<int> (frame.height * height);
    return {x, y, width_, height_};
}

pair<int, int> SDLPanel::getTextureDimensions (SDL_Texture* texture)
{
    Uint32 format;
    int access;
    int width, height;

    SDL_QueryTexture (texture, &format, &access, &width, &height);
    return make_pair (width, height);
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

bool SDLPanel::isOverButton (Position position)
{
    // Disclaimer: the for statement could be expressed with a find_if() call
    // but that makes the intent of this function unclear rather than clear.
    for (auto& button : buttons_)
       if (button->isWithinBounds (position, 0.75f))
           return true;

    return false;
}
