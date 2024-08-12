#ifndef _SDLWINDOW_H_
#define _SDLWINDOW_H_

#include "panel.h"
#include "../sdlpanel/sdlpanel.h"
#include "../sdlrenderer/sdlrenderer.h"
#include "../sdlinit/sdlinit.h"

#include <vector>
#include <utility>

using std::vector;
using std::make_pair;

// Create a Window by means of SDL.
//
// The SDL initialization is put in a separate class which we derive from so
// the windows construction code is separated from the SDL initialization
// statements.
class SDLWindow : public Window, public SDLInit
{
public:
    SDLWindow (char const *title, int x, int y, int width, int height);
    ~SDLWindow ();
    Panel *createPanel () override;
    void handler ();


private:
    // Lopu circle radius
    static const int loupeRadius_ = 50;

    int windowWidth_;
    int windowHeight_;
    int textureWidth_;
    int textureHeight_;
    bool loupeShown_ = false;
    int windowPositionX_ {0};
    int windowPositionY_ {0};
    int texturePositionX_ {0};
    int texturePositionY_ {0};

    SDL_Window* sdlWindow_;

    // The Renderer to use in rendering in the given window
    unique_ptr<SDLRenderer> sdlRenderer_; 

    vector<unique_ptr<SDLPanel>> panels_;

    // The target texture the panels have to render to
    SDL_Texture* targetTexture_;

    void render () override;
    bool handleEvents () override;

    pair<int, int> windowToTexturePosition (int windowPositionX,
        int windowPositionY);
    void RenderCopyCircle (SDL_Renderer* renderer, SDL_Texture* texture,
        int sourceCenterX, int sourceCenterY, int sourceRadius,
        int destCenterX, int destCenterY, int destRadius);
    int RenderDrawCircle (SDL_Renderer* renderer, int x, int y, int radius);
};

#endif // _SDLWINDOW_H_