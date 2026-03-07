#include "sdlpanel.h"

#include <algorithm>
#include <utility>

using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;
using std::move;
using std::string;
using std::ranges::find_if;
using std::pair;
using std::make_pair;

SDLPanel::SDLPanel (unique_ptr<SDLRenderer> &sdlRenderer,
    SDLTexture& texture, Cabinet::Position cabinetPosition, RackUnit unitHeight)
    :
    sdlRenderer_ {sdlRenderer},
    targetTexture_ {texture},
    cabinetPosition_ {cabinetPosition}
{
    static const RackUnit h9642Height {20_ru};
    auto [textureWidth, textureHeight] = targetTexture_.dimensions ();
    pixelsPerRackUnit_ = textureHeight / h9642Height;
    panelHeight_ = pixelsPerRackUnit_ * unitHeight;
}

// Create a panel from the given PanelComposition.
SDLPanel::SDLPanel (unique_ptr<SDLRenderer>& sdlRenderer,
    SDLTexture& texture, Cabinet::Position cabinetPosition,
    RackUnit unitHeight, PanelComposition panelComposition)
    :
    sdlRenderer_ {sdlRenderer},
    targetTexture_ {texture},
    cabinetPosition_ {cabinetPosition},
    fronts_ {move (panelComposition.fronts_)},
    indicators_ {move (panelComposition.indicators_)},
    buttons_ {move (panelComposition.buttons_)},
    indicatorButtons_ {move (panelComposition.indicatorButtons_)}
{ }

SDLPanel::~SDLPanel ()
{}

// Render all elements in this panel to the window
void SDLPanel::render ()
{
    // Render all fronts, indicators, buttons, etc.
    for (auto& sdlFront : fronts_)
        sdlFront->render ();

    for (auto& indicator : indicators_)
        indicator->render ();

    for (auto& button : buttons_)
        button->render ();

    for (auto& indicatorButton : indicatorButtons_)
        indicatorButton->render ();
}

// Events for a Panel are destined for a button on the panel
void SDLPanel::handleEvent (InputEvent const *event)
{
    for (auto& button : buttons_)
        button->handleEvent (event);

    for (auto& indicatorButton : indicatorButtons_)
        indicatorButton->handleEvent (event);
}

bool SDLPanel::isOverButton (Position position)
{
    // Disclaimer: the for statement could be expressed with a find_if() call
    // but that makes the intent of this function unclear rather than clear.
    for (auto& button : buttons_)
       if (button->isWithinBounds (position, 0.75f))
           return true;

    for (auto& indicatorButton : indicatorButtons_)
        if (indicatorButton->isWithinBounds (position, 0.75f))
            return true;

    for (auto& indicator : indicators_)
        if (indicator->isWithinBounds (position, 0.75f))
            return true;

    return false;
}
