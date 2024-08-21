#include "sdlpanel.h"

#include <algorithm>
#include <utility>

using std::make_unique;
using std::string;
using std::ranges::find_if;
using std::pair;
using std::make_pair;

SDLPanel::SDLPanel (unique_ptr<SDLRenderer> &sdlRenderer,
    SDL_Texture* texture, Cabinet::Position cabinetPosition, RackUnit unitHeight)
    :
    sdlRenderer_ {sdlRenderer},
    targetTexture_ {texture},
    cabinetPosition_ {cabinetPosition}
{
    static const RackUnit h9642Height {20_ru};
    auto [textureWidth, textureHeight] = getTextureDimensions (targetTexture_);
    pixelsPerRackUnit_ = textureHeight / h9642Height;
    panelHeight_ = pixelsPerRackUnit_ * unitHeight;
}

SDLPanel::~SDLPanel ()
{}

void SDLPanel::createFront (string imageFile, 
        Frame<float> frame)
{
    fronts_.push_back (make_unique<SDLFront> (imageFile, 
        sdlRenderer_, targetTexture_, placeFrameInTexture (frame)));
}

Indicator *SDLPanel::createIndicator (string imageFile,
    Indicator::State showFigure, Frame<float> frame)
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
Frame<int> SDLPanel::placeFrameInTexture (Frame<float> frame)
{
    // The passed frame contains positions relative to the target
    // texture. Calculate the frame's position and dimensions in pixels
    // from these relative values.
    auto [textureWidth, textureHeight] = getTextureDimensions (targetTexture_);
    int x = static_cast<int> (frame.x * textureWidth);
    int y = static_cast<int> (textureHeight - 
        (cabinetPosition_.height * pixelsPerRackUnit_) +
        (frame.y * panelHeight_));
    int width_ = static_cast<int> (frame.width * textureWidth);
    int height_ = static_cast<int> (frame.height * panelHeight_);
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
