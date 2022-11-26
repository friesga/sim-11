#ifndef _BA11_N_H_
#define _BA11_N_H_

#include <SDL.h>
#include <thread>
#include <memory>
#include <string>

class BA11_N
{
	SDL_Window *window_;
    SDL_Renderer *renderer_;
    SDL_Texture *texture_;
    SDL_Rect sourceRectangle_;
	SDL_Rect destinationRectangle_;
    bool running_ {false};
	std::thread ba11_nThread;

	void bezel ();
    void render();
	void update();
	void handleEvents();
	SDL_Surface *loadImage (std::string image);

public:
    BA11_N (char const *title, int xpos, int ypos, 
        int width, int height, bool fullscreen);
    ~BA11_N();

    bool isRunning ();
};

#endif // _BA11_N_H_
