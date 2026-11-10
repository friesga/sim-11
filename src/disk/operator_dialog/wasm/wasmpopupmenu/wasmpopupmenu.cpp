#include "wasmpopupmenu.h"
#include "panel.h"

#include <bind.h>    // link with -lembind
#include <val.h>

#include <SDL3/SDL.h>
#include <cstddef>
#include <string>
#include <utility>  
#include <iostream>
#include <stdexcept>

using emscripten::val;
using std::move;
using std::string;
using std::uintptr_t;
using std::runtime_error;

// Makes onResult callable from JavaScript as
// Module.WASMPopUpMenu_onResult(key, index). As a nested class,
// BindingsRegistrar may use the private member function; its constructor
// runs during static initialization when bindingsRegistrar_ is created.
WASMPopupMenu::BindingsRegistrar::BindingsRegistrar ()
{
    emscripten::function ("WASMPopUpMenu_onResult", &WASMPopupMenu::onResult);
}

const WASMPopupMenu::BindingsRegistrar WASMPopupMenu::bindingsRegistrar_;

WASMPopupMenu::WASMPopupMenu (const MenuOptionList& options,
                              PopupCallback callback)
    :
    options_ {options},
    callback_ {move (callback)}
{}

WASMPopupMenu::~WASMPopupMenu ()
{
    // JavaScript holds our address; make sure it is forgotten.
    jsPopupMenu ().call<void> ("close", jsKey ());
}

void WASMPopupMenu::open (int x, int y)
{
    SDL_Window* const parentWindow = SDL_GetMouseFocus ();
    if (parentWindow == nullptr)
        throw runtime_error (
            "SDLPopupMenu::getOption requires an SDL window with mouse focus");

    Position mousePosition = getMousePosition ();

    val labels = val::array ();
    for (const string& label : options_.strings ())
        labels.call<void> ("push", label);

    jsPopupMenu ().call<void> ("open", jsKey (),
        mousePosition.x, mousePosition.y, labels);
}

void WASMPopupMenu::selected (const MenuOption* option)
{
    // Copy the callback: it may destroy this object while running.
    PopupCallback callback = callback_;
    if (callback && option != nullptr)
        callback (option);
}

void WASMPopupMenu::onResult (uintptr_t key, int index)
{
    WASMPopupMenu* menu = reinterpret_cast<WASMPopupMenu*> (key);

    const MenuOption* result = nullptr;
    if (index >= 0 && static_cast<size_t> (index) < menu->options_.size ())
        result = &menu->options_[index];

    // Must be the last use of 'menu': the callback may delete it.
    menu->selected (result);
}

val WASMPopupMenu::jsPopupMenu ()
{
    return val::global ("WASMPopupMenu");
}

uintptr_t WASMPopupMenu::jsKey () const
{
    return reinterpret_cast<uintptr_t> (this);
}

// This function returns the position of the mouse relative to the window
// with mouse focus, which is the coordinate system SDL_GetMouseState returns.
WASMPopupMenu::Position WASMPopupMenu::getMousePosition () const
{
    float mouseX = 0.0F;
    float mouseY = 0.0F;
    SDL_GetMouseState (&mouseX, &mouseY);
    const size_t offsetX = lround (mouseX);
    const size_t offsetY = lround (mouseY);

    return Position {offsetX, offsetY};
}
