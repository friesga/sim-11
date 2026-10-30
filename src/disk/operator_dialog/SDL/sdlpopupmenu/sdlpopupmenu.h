#ifndef _SDLPOPUPMENU_H_
#define _SDLPOPUPMENU_H_

#include "../../popupmenu/menuoptionlist/menuoptionlist.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <initializer_list>
#include <optional>
#include <string>
#include <vector>
#include <memory>

class SDLPopupMenu
{
public:
    explicit SDLPopupMenu (const MenuOptionList& options);

    // Shows a modal popup at the given mouse position. SDL's video
    // subsystem must be initialized and the calling thread must be the main
    // thread. Returns the clicked option, or nullopt when dismissed.
    [[nodiscard]] virtual const MenuOption* getOption (int x, int y);

private:
    struct WindowDeleter
    {
        void operator() (SDL_Window* window) const noexcept
        {
            SDL_DestroyWindow (window);
        }
    };

    struct RendererDeleter
    {
        void operator() (SDL_Renderer* renderer) const noexcept
        {
            SDL_DestroyRenderer (renderer);
        }
    };

    using WindowPtr = std::unique_ptr<SDL_Window, WindowDeleter>;
    using RendererPtr = std::unique_ptr<SDL_Renderer, RendererDeleter>;

    struct Dimensions
    {
        size_t width;
        size_t height;
    };

    struct Position
    {
        size_t x;
        size_t y;
    };

    struct Colour
    {
        Uint8 r;
        Uint8 g;
        Uint8 b;
    };

    class SDLPopUpMenuSession
    {
    public:
        SDLPopUpMenuSession (
            SDL_Window* parentWindow,
            Position mousePosition,
            Dimensions rowDimensions,
            Dimensions windowDimensions,
            const MenuOptionList& options);

        const MenuOption* getResult ();

    private:
        struct FontDeleter
        {
            void operator() (TTF_Font* font) const
            {
                TTF_CloseFont (font);
            }
        };

        struct SurfaceDeleter
        {
            void operator() (SDL_Surface* surface) const
            {
                SDL_DestroySurface (surface);
            }
        };

        struct TextureDeleter
        {
            void operator() (SDL_Texture* texture) const
            {
                SDL_DestroyTexture (texture);
            }
        };

        using FontPtr = std::unique_ptr<TTF_Font, FontDeleter>;
        using SurfacePtr = std::unique_ptr<SDL_Surface, SurfaceDeleter>;
        using TexturePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;

        static constexpr float fontSize {12.0};

        SDL_Window* parentWindow_;
        Position mousePosition_;
        Dimensions rowDimensions_;
        Dimensions windowDimensions_;
        const MenuOptionList& options_;

        WindowPtr window_;
        RendererPtr renderer_;
        FontPtr font_;
        SDL_WindowID windowID_ {0};
        std::optional<size_t> highlightedOption_ {std::nullopt};
        const MenuOption* result_ {nullptr};

        void createWindowAndRenderer (SDL_Window* parentWindow);
        void openPopUp ();
        void drawMenu (SDL_Renderer* renderer, const std::vector<std::string>& options,
            Dimensions windowDimensions, std::optional<size_t> highlightedOption) const;
        void setColour (SDL_Renderer* renderer, Colour color) const;
        void renderText (SDL_Renderer* renderer, const std::string& text,
            float x, float y) const;
        std::optional<size_t> optionAt (float x, float y, int width) const noexcept;
        void highlightOption (SDL_Event event);
        void clearHighlightedOption ();
        const MenuOption* getOption (SDL_Event event);
        [[noreturn]] void throwSDLError (const char* operation) const;
        void openFontFile ();
    };
    
    Dimensions calculateRowDimensions () const;
    Dimensions calculateWindowDimensions (Dimensions rowDimensions) const;
    Position getMousePosition () const;

    // Constants defining the appearance of the popup menu.
    static constexpr size_t borderWidth = 2;
    static constexpr size_t horizontalPadding = 10;
    static constexpr size_t rowHeight = 24;
    static constexpr size_t minimumRowWidth = 80;

    Dimensions rowDimensions_;
    Dimensions windowDimensions_ {0, 0};
    const MenuOptionList& options_;
};

#endif // _SDLPOPUPMENU_H_