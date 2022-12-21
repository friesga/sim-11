#ifndef _BA11_N_H_
#define _BA11_N_H_

#include "qbus/qbus.h"
#include "../sdl/sdlwrap/sdlwrap.h"
#include "../sdl/texture/texture.h"
#include "../sdl/window/window.h"
#include "../sdl/renderer/renderer.h"

#include <SDL.h>
#include <thread>
#include <memory>
#include <string>

using std::unique_ptr;

class BA11_N
{
    QBUS &bus_;

    // SDL_Texture *texture_;
    SDL_Rect sourceRectangle_;
	SDL_Rect destinationRectangle_;

    bool running_ {false};
	std::thread ba11_nThread_;

    unique_ptr<Window> window_;
    unique_ptr<Renderer> renderer_;
    unique_ptr<Texture> pdp11_03_frontTexture;
    unique_ptr<Texture> pwrOkTexture_;
    unique_ptr<Texture> runLightTexture_;

    void init (char const *title, int xpos, int ypos, 
        int width, int height, bool fullscreen);
	void bezel ();
    void render();
	void update();
	void handleEvents();
	SDL_Surface *loadImage (std::string image);

public:
    BA11_N (QBUS &bus);
    ~BA11_N();

    bool isRunning ();
};

inline bool BA11_N::isRunning ()
{
    return running_;
}

#endif // _BA11_N_H_
