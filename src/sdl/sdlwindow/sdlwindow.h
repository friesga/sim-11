#ifndef _SDLWINDOW_H_
#define _SDLWINDOW_H_

#include "panel.h"
#include "sdl/sdlpanel/sdlpanel.h"
#include "sdl/sdlrenderer/sdlrenderer.h"
#include "sdl/sdlinit/sdlinit.h"
#include "sdl/sdlgraphicscontext/sdlgraphicscontext.h"
#include "cabinet/cabinet.h"

#include <vector>
#include <utility>
#include <set>
#include <memory>

using std::vector;
using std::make_pair;
using std::set;
using std::unique_ptr;

// Create a Window by means of SDL.
//
// The SDL initialization is put in a separate class which we derive from so
// the windows construction code is separated from the SDL initialization
// statements.
class SDLWindow : public Window, public SDLInit
{
public:
    SDLWindow (char const *title, Frame<int> frame,
        set<Window::Flag> flags = {});
    ~SDLWindow ();
    void show () override;
    Panel *createPanel (Cabinet::Position cabinetPosition,
        RackUnit unitHeight) override;
    void handler ();

private:
    // Loupe circle radius
    static const int loupeRadius_ = 50;

    int windowWidth_;
    int windowHeight_;
    int textureWidth_;
    int textureHeight_;
    bool showLoupe_ = false;
    Position windowPosition_ {0, 0};
    Position texturePosition_ {0, 0};

    SDL_Window* sdlWindow_;

    // The Renderer to use in rendering in the given window
    unique_ptr<SDLRenderer> sdlRenderer_; 

    vector<unique_ptr<SDLPanel>> panels_;

    // The target texture the panels have to render to
    SDL_Texture* targetTexture_;

    // (Pointer to) the SDL graphics context to be use in rendering the
    // textures
    unique_ptr<SDLGraphicsContext> graphicsContext_;

    // Definition of a cabinet to keep track of occupied positions in
    // the cabinet. 
    Cabinet h9642Cabinet {"h9642", 20_ru};

    void render () override;
    bool handleEvents () override;

    Position windowToTexturePosition (Position windowPosition);
    void RenderCopyCircle (SDL_Renderer* renderer, SDL_Texture* texture,
        Position sourceCenter, int sourceRadius,
        Position destCenter, int destRadius);
    int RenderDrawCircle (SDL_Renderer* renderer, Position position,
        int radius);
    void drawLoupe ();
};

#endif // _SDLWINDOW_H_