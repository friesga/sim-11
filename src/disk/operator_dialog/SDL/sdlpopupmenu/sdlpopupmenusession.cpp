#include "sdlpopupmenu.h"

#include <SDL3/SDL.h>
#include <stdexcept>
#include <optional>
#include <iostream>

using std::runtime_error;
using std::string;
using std::optional;
using std::nullopt;
using std::vector;
using std::string;
using std::runtime_error;

SDLPopupMenu::SDLPopUpMenuSession::SDLPopUpMenuSession (
    SDL_Window* parentWindow,
    Position mousePosition,
    Dimensions rowDimensions,
    Dimensions windowDimensions,
    const MenuOptionList& options)
    :
    parentWindow_ {parentWindow},
    mousePosition_ {mousePosition},
    rowDimensions_ {rowDimensions},
    windowDimensions_ {windowDimensions},
    options_ {options}
{
    openFontFile ();
    createWindowAndRenderer (parentWindow);
    openPopUp ();
}

void SDLPopupMenu::SDLPopUpMenuSession::openFontFile ()
{
    string executableLocation = SDL_GetBasePath ();
    string fontFile = executableLocation + "/fonts/DejaVuSans.ttf";
    font_.reset (TTF_OpenFont (fontFile.c_str (), fontSize));

    if (font_ == NULL)
        throwSDLError ("TTF_OpenFont");
}

const MenuOption* SDLPopupMenu::SDLPopUpMenuSession::getResult ()
{
    return result_;
}

void SDLPopupMenu::SDLPopUpMenuSession::createWindowAndRenderer (SDL_Window* parentWindow)
{
    window_.reset (SDL_CreatePopupWindow (parentWindow,
        mousePosition_.x, mousePosition_.y,
        windowDimensions_.width,
        windowDimensions_.height,
        SDL_WINDOW_POPUP_MENU |
        SDL_WINDOW_HIDDEN));

    if (window_ == nullptr)
        throwSDLError ("SDL_CreatePopupWindow");

    renderer_.reset (SDL_CreateRenderer (window_.get (), nullptr));
    if (renderer_ == nullptr)
        throw runtime_error (string ("SDL_CreateRenderer") + SDL_GetError ());

    if (!SDL_ShowWindow (window_.get ()))
        throwSDLError ("SDL_ShowWindow");

    windowID_ = SDL_GetWindowID (window_.get ());
    if (windowID_ == 0)
        throwSDLError ("SDL_GetWindowID");
}

void SDLPopupMenu::SDLPopUpMenuSession::openPopUp ()
{
    drawMenu (renderer_.get (), options_.strings (), windowDimensions_,
        highlightedOption_);

    bool finished = false;
    optional<SDL_Event> quitEvent {nullopt};

    while (!finished)
    {
        SDL_Event event {};
        if (!SDL_WaitEvent (&event))
            throwSDLError ("SDL_WaitEvent");

        switch (event.type)
        {
            case SDL_EVENT_MOUSE_MOTION:
                if (event.motion.windowID == windowID_)
                    highlightOption (event);
                break;

            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                if (event.window.windowID == windowID_)
                    clearHighlightedOption ();
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.windowID == windowID_)
                    result_ = getOption (event);
                finished = true;
                break;

            case SDL_EVENT_KEY_DOWN:
                if (event.key.windowID == windowID_ && event.key.key == SDLK_ESCAPE)
                    finished = true;
                break;

            case SDL_EVENT_WINDOW_FOCUS_LOST:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            case SDL_EVENT_WINDOW_HIDDEN:
                if (event.window.windowID == windowID_)
                    finished = true;
                break;

            case SDL_EVENT_QUIT:
                // The application still needs to see its quit request after this
                // temporary modal event loop has closed.
                quitEvent = event;
                finished = true;
                break;

            default:
                break;
        }
    }

    if (quitEvent)
        SDL_PushEvent (&quitEvent.value ());
}

void SDLPopupMenu::SDLPopUpMenuSession::drawMenu (SDL_Renderer* renderer,
    const vector<string>& options,
    Dimensions windowDimensions,
    optional<size_t> highlightedOption) const
{
    setColour (renderer, Colour {245, 245, 245});
    if (!SDL_RenderClear (renderer))
        throwSDLError ("SDL_RenderClear");

    for (size_t i = 0; i < options.size (); ++i)
    {
        const float rowY = static_cast<float>
            (borderWidth + i * rowDimensions_.height);
        const SDL_FRect row
        {
            static_cast<float> (borderWidth),
            rowY,
            static_cast<float> (rowDimensions_.width - 2 * borderWidth),
            static_cast<float> (rowDimensions_.height)
        };

        const bool highlighted =
            highlightedOption && i == *highlightedOption;

        const Colour backgroundColour =
            highlighted ? Colour {49, 106, 197}
                        : Colour {225, 225, 225};

        const Colour textColour =
            highlighted ? Colour {255, 255, 255}
                        : Colour {25, 25, 25};

        setColour (renderer, backgroundColour);
        if (!SDL_RenderFillRect (renderer, &row))
            throw runtime_error (string ("SDL_RenderFillRect: ") + SDL_GetError ());

        const float textY = rowY +
            (rowHeight - fontSize) / 2.0F;
        setColour (renderer, textColour);
        renderText (renderer, options [i],
            static_cast<float> (borderWidth + horizontalPadding), textY);
        
        setColour (renderer, Colour {120, 120, 120});
        if (!SDL_RenderRect (renderer, &row))
            throw runtime_error (string ("SDL_RenderRect: ") + SDL_GetError ());
    }

    setColour (renderer, Colour {35, 35, 35});
    const SDL_FRect outline
    {
        0.5F,
        0.5F,
        static_cast<float>(windowDimensions.width - 1),
        static_cast<float>(windowDimensions.height - 1)
    };
    if (!SDL_RenderRect (renderer, &outline))
        throwSDLError ("SDL_RenderRect");
    if (!SDL_RenderPresent (renderer))
        throwSDLError ("SDL_RenderPresent");
}

void SDLPopupMenu::SDLPopUpMenuSession::setColour (SDL_Renderer* renderer, Colour colour) const
{
    if (!SDL_SetRenderDrawColor (renderer, colour.r, colour.g, colour.b, SDL_ALPHA_OPAQUE))
        throwSDLError ("SDL_SetRenderDrawColor");
}

void SDLPopupMenu::SDLPopUpMenuSession::renderText (SDL_Renderer* renderer,
    const string& text, float x, float y) const
{
    const SDL_Color textColor {0, 0, 0, 255};

    SurfacePtr surface {
        TTF_RenderText_Blended (
            font_.get (), text.c_str (), 0, textColor)
    };

    if (!surface)
        throwSDLError ("TTF_RenderText_Blended");

    TexturePtr texture {
        SDL_CreateTextureFromSurface (renderer, surface.get ())
    };

    if (!texture)
        throwSDLError ("SDL_CreateTextureFromSurface");

    const SDL_FRect destination {
        x,
        y,
        static_cast<float>(surface->w),
        static_cast<float>(surface->h)
    };

    if (!SDL_RenderTexture (
        renderer, texture.get (), nullptr, &destination))
        throwSDLError ("SDL_RenderTexture");
}

optional<size_t> SDLPopupMenu::SDLPopUpMenuSession::optionAt (float x,
    float y, int width) const noexcept
{
    if (x < static_cast<float> (borderWidth) ||
        x >= static_cast<float> (width - borderWidth) ||
        y < static_cast<float> (borderWidth))
    {
        return nullopt;
    }

    const size_t index = (y - borderWidth) / rowDimensions_.height;
    return index;
}

void SDLPopupMenu::SDLPopUpMenuSession::highlightOption (SDL_Event event)
{
    const optional<size_t> hoveredOption = optionAt (event.motion.x,
        event.motion.y, windowDimensions_.width);
    if (hoveredOption != highlightedOption_)
    {
        highlightedOption_ = hoveredOption;
        drawMenu (renderer_.get (), options_.strings (), windowDimensions_,
            highlightedOption_);
    }
}

void SDLPopupMenu::SDLPopUpMenuSession::clearHighlightedOption ()
{
    if (highlightedOption_)
    {
        highlightedOption_ = nullopt;
        drawMenu (renderer_.get (), options_.strings (), windowDimensions_,
            highlightedOption_);
    }
}

const MenuOption* SDLPopupMenu::SDLPopUpMenuSession::getOption (SDL_Event event)
{
    const MenuOption* result {nullptr};

    if (event.button.button == SDL_BUTTON_LEFT)
    {
        // A left click either chooses a row or dismisses the menu.
        const optional<size_t> clickedOption = optionAt (event.button.x,
            event.button.y,
            windowDimensions_.width);
        if (clickedOption)
            result = &options_[*clickedOption];
    }
    return result;
}

[[noreturn]] void SDLPopupMenu::SDLPopUpMenuSession::throwSDLError (const char* operation) const
{
    throw runtime_error (string (operation) + ": " + SDL_GetError ());
}