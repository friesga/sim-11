#include "operator_dialog/popupmenu/menuoption/menuoption.h"
#include "sdlpopupmenu.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <optional>

using std::size_t;
using std::optional;
using std::nullopt;
using std::vector;
using std::string;
using std::move;
using std::initializer_list;
using std::runtime_error;
using std::max;
using std::lround;

SDLPopupMenu::SDLPopupMenu (const MenuOptionList& options)
    : 
    options_ (move (options))
{
    rowDimensions_ = calculateRowDimensions ();
    windowDimensions_ = calculateWindowDimensions (rowDimensions_);
}


const MenuOption* SDLPopupMenu::getOption (int x, int y)
{
    if (options_.size () == 0)
        return nullptr;
    
    SDL_Window* const parentWindow = SDL_GetMouseFocus ();
    if (parentWindow == nullptr)
        throw runtime_error (
            "SDLPopupMenu::getOption requires an SDL window with mouse focus");

    Position mousePosition = getMousePosition ();
    // Position mousePosition {static_cast<size_t> (x), static_cast<size_t> (y)};

    SDLPopUpMenuSession session {parentWindow, mousePosition, rowDimensions_,
        windowDimensions_, options_};

    return session.getResult ();
}

SDLPopupMenu::Dimensions SDLPopupMenu::calculateRowDimensions () const
{
    size_t longest = 0;
    for (const auto& option : options_.strings ())
        longest = max (longest, option.size ());

    constexpr size_t characterWidth = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    const size_t requestedWidth = longest * characterWidth +
        2U * (horizontalPadding + borderWidth);

    const size_t width = max (minimumRowWidth, requestedWidth);
    return Dimensions {width, rowHeight};
}

SDLPopupMenu::Dimensions SDLPopupMenu::calculateWindowDimensions (Dimensions rowDimensions) const
{
    const size_t height = 2 * borderWidth + options_.size () * rowDimensions.height;

    return Dimensions {rowDimensions.width, height};
}

// This function returns the position of the mouse relative to the window
// with mouse focus, which is the coordinate system SDL_GetMouseState returns.
SDLPopupMenu::Position SDLPopupMenu::getMousePosition () const
{
    float mouseX = 0.0F;
    float mouseY = 0.0F;
    SDL_GetMouseState (&mouseX, &mouseY);
    const size_t offsetX = lround (mouseX);
    const size_t offsetY = lround (mouseY);

    return Position {offsetX, offsetY};
}